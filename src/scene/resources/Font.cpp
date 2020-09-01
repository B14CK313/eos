//
// Created by jakob on 26.08.20.
//

#include <utf8.h>
#include <spdlog/spdlog.h>
#include "eos/scene/resources/Font.hpp"

eos::Font::Font(const std::string_view path, float size) : path_(path) {
    /*
    FT_Matrix matrix = {static_cast<FT_Fixed>((1.0f/horRes_) * 0x10000L),0,0, 0x10000L};

    if(FT_Error err = FT_New_Face(eos::ServiceProvider::getFreetype(), path.data(), 0, &face_)) {
        SPDLOG_ERROR("FT_New_Face Error, code {}", err);
        throw;
    }

    if(FT_Error err = FT_Select_Charmap(face_, FT_ENCODING_UNICODE)){
        SPDLOG_ERROR("FT_Select_Charmap Error, code {}", err);
        FT_Done_Face(face_);
        throw;
    }

    if(FT_Error err = FT_Set_Char_Size(face_, 100 * size * horRes_, 0, dpi_ * horRes_, dpi_)){
        SPDLOG_ERROR("FT_Set_Char_Size Error, code {}", err);
        FT_Done_Face(face_);
        throw;
    }

    FT_Set_Transform(face_, &matrix, nullptr);
     */

    msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
    if(ft) {
        font_ = msdfgen::loadFont(ft, path_.data());
    }

    shader_ = std::make_shared<Shader>("res/text.vert", "res/text.frag");
    shader_->use();
    shader_->set_vec2_uniform("scale", 1.0f, 1.0f);
    shader_->set_vec2_uniform("offset", 0.0f, 0.0f);
    shader_->set_float_uniform("pxRange", 1.0f);
    shader_->set_vec4_uniform("bgColor", 0.0f, 0.0f, 0.0f, 0.0f);
    shader_->set_vec4_uniform("fgColor", 1.0f, 1.0f, 1.0f, 1.0f);
    shader_->set_int_uniform("textureCache", 1);
}

void eos::Font::cache(std::string_view chars) {
    auto start = chars.begin();
    auto end = chars.end();
    while (start < end) {
        unsigned int codepoint = utf8::next(start, end);
        SPDLOG_TRACE("Caching {} ({})", codepoint, *(start - 1));
        cache(*(start - 1), codepoint);
    }
}

void eos::Font::cache(char c, uint32_t codepoint) {
    if (auto it = glyphMap_.find(codepoint); it != glyphMap_.end()) {
        // already cached
    } else {
/*
        FT_UInt glyphIndex = FT_Get_Char_Index(face_, codepoint);
        if(glyphIndex == 0) {
            SPDLOG_WARN("Character {} index is 0. Will not render.", codepoint);
        }

        if(FT_Error err = FT_Load_Glyph(face_, glyphIndex, FT_LOAD_NO_SCALE | FT_LOAD_RENDER | FT_LOAD_TARGET_LCD)){ // LOAD_TARGET_LCD?
            SPDLOG_ERROR("FT_Load_Char Error, code {}", err);
            FT_Done_Face(face_);
            return;
        }

        if(FT_Error err = FT_Render_Glyph(face_->glyph, FT_RENDER_MODE_NORMAL)){
            SPDLOG_ERROR("FT_Render_Glyph Error, code {}", err);
            FT_Done_Face(face_);
            return;
        }


        FT_GlyphSlot slot = face_->glyph;
        FT_Bitmap bitmap = slot->bitmap;
        glm::uvec4 padding{1, 1, 1, 1};

        glm::uvec2 srcDim = {bitmap.width, bitmap.rows};
        glm::uvec2 tgtDim = {srcDim.x + padding.x + padding.z, srcDim.y + padding.y + padding.w};
*/


        msdfgen::Shape shape;
        if (msdfgen::loadGlyph(shape, font_, codepoint)) {
            shape.normalize();
            msdfgen::edgeColoringSimple(shape, 3.0);
            auto bounds = shape.getBounds();
            glm::uvec2 dims = {std::ceil(std::abs(bounds.l) + bounds.r), std::ceil(std::abs(bounds.b) + bounds.t)};
            SPDLOG_DEBUG("{} {} {} {} {} {}", bounds.l, bounds.r, bounds.b, bounds.t, dims.x, dims.y);
            msdfgen::Bitmap<float, 3> msdf(dims.x, dims.y);
            msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(0, -bounds.b));
            glyphMap_.emplace(c, textureAtlas_.insert(static_cast<const float*>(msdf), dims));
//                    SPDLOG_DEBUG("TEST {} {}", codepoint, msdf.width());
        }
    }
}

void eos::Font::render(std::string_view text, glm::vec2 pos) {
    //textureAtlas_.render(pos, 1.0f);

    shader_->use();

    struct Glyph{
        glm::vec4 bl;
        glm::vec4 br;
        glm::vec4 tl;
        glm::vec4 tr;
    };

    struct Indices {
        GLuint bl1;
        GLuint br1;
        GLuint tl1;
        GLuint br2;
        GLuint tl2;
        GLuint tr2;
    };

    std::vector<Glyph> glyphs(text.length());
    std::vector<Indices> indices(text.length());

    for(size_t i = 0; i < text.length(); ++i){
        glm::vec4 dim = textureAtlas_[glyphMap_[text[i]]];
        dim = {dim.x / 512, dim.y / 512, dim.z / 512, dim.w / 512};

        glyphs[i].bl = {pos.x + i*0.1, pos.y, dim.x, dim.y};
        glyphs[i].br = {pos.x + i*0.1 + 0.1, pos.y, dim.x + dim.z, dim.y};
        glyphs[i].tl = {pos.x + i*0.1, pos.y + 0.1, dim.x, dim.y + dim.w};
        glyphs[i].tr = {pos.x + i*0.1 + 0.1, pos.y + 0.1, dim.x + dim.z, dim.y + dim.w};

        size_t idxOffset = i * 4;
        indices[i].bl1 = 0 + idxOffset;
        indices[i].br1 = 1 + idxOffset;
        indices[i].tl1 = 2 + idxOffset;
        indices[i].br2 = 1 + idxOffset;
        indices[i].tl2 = 2 + idxOffset;
        indices[i].tr2 = 3 + idxOffset;
    }

    GLuint textVAO;
    GLuint textVBO;
    GLuint textIBO;
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, textureAtlas_.texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glGenBuffers(1, &textIBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIBO);

    glBufferData(GL_ARRAY_BUFFER, text.length() * sizeof(Glyph), &glyphs[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, text.length() * sizeof(Indices), &indices[0], GL_STATIC_DRAW);
    //int positionLocation = shader_->get_attribute_location("position");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
    //int texCoordLocation = shader_->get_attribute_location("texCoord");
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<GLvoid*>(sizeof(glm::vec4)/2));

    glBindSampler(1, 0);
    glBindVertexArray(textVAO);
    glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_INT, nullptr);
}