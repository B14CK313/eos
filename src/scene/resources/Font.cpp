//
// Created by jakob on 26.08.20.
//

#include <glad/glad.h>
#include <hb-ft.h>
#include <spdlog/spdlog.h>
#include <eos/core/ServiceProvider.h>
#include <eos/scene/resources/Shader.h>
#include "eos/scene/resources/Font.hpp"

eos::Font::Font(std::string_view path, size_t pixelSize, size_t cacheSize) : path_(path) {
    if(FT_Error error = FT_New_Face(eos::ServiceProvider::getFreetype(), path_.data(), 0, &face_)){
        SPDLOG_ERROR("FT_New_Face error: {}", error);
    }

    if(FT_Error error = FT_Set_Pixel_Sizes(face_, 0, pixelSize)){
        FT_Done_Face(face_);
        SPDLOG_ERROR("FT_Set_Pixel_Sizes error: {}", error);
    }

    font_ = hb_ft_font_create(face_, nullptr);

    size_t maxSlotWidth = static_cast<size_t>(ceil((face_->max_advance_width * face_->size->metrics.y_ppem) / static_cast<float>(face_->units_per_EM)));
    size_t maxSlotHeight = static_cast<size_t>(ceil((face_->height * face_->size->metrics.y_ppem) / static_cast<float>(face_->units_per_EM)));

    cacheDim_ = {256, 256};
    cacheLineHeight_ = 0;
    cacheFreeSlotPos_ = {0, 0};

    SPDLOG_DEBUG("eos::Font::Font({}, {}): {}x{} (cache {}x{})", path_, pixelSize, maxSlotWidth, maxSlotHeight, cacheDim_.x, cacheDim_.y);

    // For cache debug draw
    // ^ y/t
    // |
    // 2 - 3
    // | \ |
    // 0 - 1 -> x/s
    GlyphVertices glyphVertices;

    glyphVertices.bl_.x = -1.0f;
    glyphVertices.bl_.y = -1.0f;
    glyphVertices.bl_.s = 0.0f;
    glyphVertices.bl_.t = 1.0f;

    glyphVertices.br_.x = 1.0f;
    glyphVertices.br_.y = -1.0f;
    glyphVertices.br_.s = 1.0f;
    glyphVertices.br_.t = 1.0f;

    glyphVertices.tl_.x = -1.0f;
    glyphVertices.tl_.y = 1.0f;
    glyphVertices.tl_.s = 0.0f;
    glyphVertices.tl_.t = 0.0f;

    glyphVertices.tr_.x = 1.0f;
    glyphVertices.tr_.y = 1.0f;
    glyphVertices.tr_.s = 1.0f;
    glyphVertices.tr_.t = 0.0f;

    GlyphIndices glyphIndices;
    glyphIndices.bl1_ = 0;
    glyphIndices.br1_ = 1;
    glyphIndices.tl1_ = 2;
    glyphIndices.br2_ = 1;
    glyphIndices.tl2_ = 2;
    glyphIndices.tr2_ = 3;

    shader_ = std::make_shared<eos::Shader>("res/text.vert", "res/text.frag");
    shader_->use();
    glGenVertexArrays(1, &cacheVAO_);
    glGenBuffers(1, &cacheVBO_);
    glGenBuffers(1, &cacheIBO_);
    glBindVertexArray(cacheVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, cacheVBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cacheIBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glyphVertices), &(glyphVertices), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glyphIndices), &(glyphIndices), GL_STATIC_DRAW);
    int positionLocation = shader_->get_attribute_location("position");
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), nullptr);
    int texCoordLocation = shader_->get_attribute_location("texCoord");
    glEnableVertexAttribArray(texCoordLocation);
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2));
    shader_->set_int_uniform("textureCache", 15);

    // cache Texture
    glActiveTexture(GL_TEXTURE0 + 15);
    glGenTextures(1, &cacheTexture_);
    glBindTexture(GL_TEXTURE_2D, cacheTexture_);
    std::vector<GLubyte> emptyData(cacheDim_.x * cacheDim_.y * sizeof(float), 0); //transparent black texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, cacheDim_.x, cacheDim_.y, 0, GL_RED, GL_UNSIGNED_BYTE, &emptyData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

eos::Font::~Font() {
    hb_font_destroy(font_);
    glDeleteTextures(1, &cacheTexture_);
    glDeleteVertexArrays(1, &cacheVAO_);
    glDeleteBuffers(1, &cacheVBO_);
    glDeleteBuffers(1, &cacheIBO_);
}


float eos::Font::cache(const std::string_view string) {
    SPDLOG_DEBUG("Caching {}", string);
    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_add_utf8(buffer, string.data(), string.size(), 0, string.size());
    hb_shape(font_, buffer, nullptr, 0);

    size_t length = hb_buffer_get_length(buffer);
    hb_glyph_info_t* glyphs = hb_buffer_get_glyph_infos(buffer, nullptr);

    glActiveTexture(GL_TEXTURE0 + 15); // TODO: Request free Texture id
    glBindTexture(GL_TEXTURE_2D, cacheTexture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for(size_t i = 0; i < length; ++i) {
        auto glyph = cacheGlyphIdxMap_.find(glyphs[i].codepoint);
        if(cacheGlyphIdxMap_.end() == glyph) {
            cache(glyphs[i].codepoint);
        }
    }

    return cacheUsage();
}

void eos::Font::cache(FT_UInt codepoint) {
    const size_t idxInCache = cacheGlyphIdxMap_.size();
    const FT_Bitmap_& bitmap = face_->glyph->bitmap;

    if(FT_Error error = FT_Load_Glyph(face_, codepoint, FT_LOAD_RENDER)){
        SPDLOG_ERROR("Error loading glyph {}", error);
    }

    SPDLOG_TRACE("eos::Font::cache({}): width={} rows={} pitch={} advance.x={}", codepoint, bitmap.width, bitmap.rows, bitmap.pitch, face_->glyph->advance.x >> 6);

    if(cacheDim_.y <= cacheFreeSlotPos_.y + bitmap.rows) {
        SPDLOG_ERROR("Cache overflow");
    }

    int pitch = bitmap.pitch;
    if(pitch < 0) pitch = -pitch;

    glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch);

    glTexSubImage2D(GL_TEXTURE_2D, 0, cacheFreeSlotPos_.x, cacheFreeSlotPos_.y, bitmap.width, bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);

    GlyphVertices glyphVertices;

    glm::vec2 offset{face_->glyph->bitmap_left, face_->glyph->bitmap_top - bitmap.rows};

    glyphVertices.bl_.x = offset.x;
    glyphVertices.bl_.y = offset.y;
    glyphVertices.bl_.s = cacheFreeSlotPos_.x / static_cast<float>(cacheDim_.x);
    glyphVertices.bl_.t = (cacheFreeSlotPos_.y + bitmap.rows) / static_cast<float>(cacheDim_.y);

    glyphVertices.br_.x = offset.x + bitmap.width;
    glyphVertices.br_.y = offset.y;
    glyphVertices.br_.s = (cacheFreeSlotPos_.x + bitmap.width) / static_cast<float>(cacheDim_.x);
    glyphVertices.br_.t = (cacheFreeSlotPos_.y + bitmap.rows) / static_cast<float>(cacheDim_.y);

    glyphVertices.tl_.x = offset.x;
    glyphVertices.tl_.y = offset.y + bitmap.rows;
    glyphVertices.tl_.s = cacheFreeSlotPos_.x / static_cast<float>(cacheDim_.x);
    glyphVertices.tl_.t = cacheFreeSlotPos_.y / static_cast<float>(cacheDim_.y);

    glyphVertices.tr_.x = offset.x + bitmap.width;
    glyphVertices.tr_.y = offset.y + bitmap.rows;
    glyphVertices.tr_.s = (cacheFreeSlotPos_.x + bitmap.width) / static_cast<float>(cacheDim_.x);
    glyphVertices.tr_.t = cacheFreeSlotPos_.y / static_cast<float>(cacheDim_.y);

    cacheGlyphVertList_.push_back(glyphVertices);
    cacheGlyphIdxMap_[codepoint] = idxInCache;

    if(bitmap.rows > cacheLineHeight_){
        cacheLineHeight_ = bitmap.rows;
    }

    cacheFreeSlotPos_.x += bitmap.width + 1;
    if(cacheDim_.x <= cacheFreeSlotPos_.x) {
        cacheFreeSlotPos_.y += cacheLineHeight_;
        cacheFreeSlotPos_.x = 0;
        cacheLineHeight_ = 0;
    }
}

eos::Text eos::Font::assemble(const std::string_view string) const {
    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_add_utf8(buffer, string.data(), string.size(), 0, string.size());
    hb_shape(font_, buffer, nullptr, 0);

    size_t length = hb_buffer_get_length(buffer);
    hb_glyph_info_t* glyphs = hb_buffer_get_glyph_infos(buffer, nullptr);
    hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buffer, nullptr);

    glActiveTexture(GL_TEXTURE0 + 15); // TODO: Request free Texture id
    glBindTexture(GL_TEXTURE_2D, cacheTexture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GlyphVertVector vertVector(length);
    GlyphIdxVector idxVector(length);

    glm::vec<2, size_t, glm::defaultp> pos{0, 0};

    for(size_t i = 0; i < length; ++i) {
        size_t idxInCache;

        auto glyph = cacheGlyphIdxMap_.find(glyphs[i].codepoint);
        if(cacheGlyphIdxMap_.end() != glyph){
            idxInCache = glyph->second;
        } else {
            SPDLOG_ERROR("Missing glyph from the cache");
        }

        vertVector[i].bl_.x = cacheGlyphVertList_[idxInCache].bl_.x + pos.x + positions[i].x_offset;
        vertVector[i].bl_.y = cacheGlyphVertList_[idxInCache].bl_.y + pos.y + positions[i].y_offset;
        vertVector[i].bl_.s = cacheGlyphVertList_[idxInCache].bl_.s;
        vertVector[i].bl_.t = cacheGlyphVertList_[idxInCache].bl_.t;

        vertVector[i].br_.x = cacheGlyphVertList_[idxInCache].br_.x + pos.x + positions[i].x_offset;
        vertVector[i].br_.y = cacheGlyphVertList_[idxInCache].br_.y + pos.y + positions[i].y_offset;
        vertVector[i].br_.s = cacheGlyphVertList_[idxInCache].br_.s;
        vertVector[i].br_.t = cacheGlyphVertList_[idxInCache].br_.t;

        vertVector[i].tl_.x = cacheGlyphVertList_[idxInCache].tl_.x + pos.x + positions[i].x_offset;
        vertVector[i].tl_.y = cacheGlyphVertList_[idxInCache].tl_.y + pos.y + positions[i].y_offset;
        vertVector[i].tl_.s = cacheGlyphVertList_[idxInCache].tl_.s;
        vertVector[i].tl_.t = cacheGlyphVertList_[idxInCache].tl_.t;

        vertVector[i].tr_.x = cacheGlyphVertList_[idxInCache].tr_.x + pos.x + positions[i].x_offset;
        vertVector[i].tr_.y = cacheGlyphVertList_[idxInCache].tr_.y + pos.y + positions[i].y_offset;
        vertVector[i].tr_.s = cacheGlyphVertList_[idxInCache].tr_.s;
        vertVector[i].tr_.t = cacheGlyphVertList_[idxInCache].tr_.t;

        size_t idxOffset = i * 4;
        idxVector[i].bl1_ = 0 + idxOffset;
        idxVector[i].br1_ = 1 + idxOffset;
        idxVector[i].tl1_ = 2 + idxOffset;
        idxVector[i].br2_ = 1 + idxOffset;
        idxVector[i].tl2_ = 2 + idxOffset;
        idxVector[i].tr2_ = 3 + idxOffset;

        pos.x += positions[i].x_advance >> 6;
    }

    shader_->use();
    GLuint textVAO;
    GLuint textVBO;
    GLuint textIBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glGenBuffers(1, &textIBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIBO);

    glBufferData(GL_ARRAY_BUFFER, length * sizeof(GlyphVertices), &vertVector[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(GlyphIndices), &idxVector[0], GL_STATIC_DRAW);
    int positionLocation = shader_->get_attribute_location("position");
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), nullptr);
    int texCoordLocation = shader_->get_attribute_location("texCoord");
    glEnableVertexAttribArray(texCoordLocation);
    glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2));

    return Text(length, shader_, textVAO, textVBO, textIBO, cacheTexture_);
}

void eos::Font::draw_cache(glm::vec2 pos, glm::vec2 scale) {
    shader_->use();
    shader_->set_vec2_uniform("offset", pos);
    shader_->set_vec2_uniform("scale", scale);
    shader_->set_vec3_uniform("color", 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0 + 15);
    glBindTexture(GL_TEXTURE_2D, cacheTexture_);
    // Bind to sampler name zero == the currently bound texture's sampler state becomes active (no dedicated sampler)
    glBindSampler(15, 0);

    // Draw the cache texture
    glBindVertexArray(cacheVAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

float eos::Font::cacheUsage() const {
    size_t pixelsUsedPrecedingLine = cacheDim_.x * cacheFreeSlotPos_.y;
    size_t pixelsUsedCurrentLine = cacheFreeSlotPos_.x * cacheLineHeight_;
    size_t pixelsTotal = cacheDim_.x * cacheDim_.y;
    return ((pixelsUsedPrecedingLine + pixelsUsedCurrentLine) / static_cast<float>(pixelsTotal));
}