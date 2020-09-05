//
// Created by jakob on 26.08.20.
//

#include <utf8.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <eos/core/ServiceProvider.h>
#include <eos/core/ColorHSV.hpp>
#include "eos/scene/resources/Font.hpp"

eos::Font::Font(const std::string& path, float size) : path_(path) {
    if (FT_Error err = FT_New_Face(eos::ServiceProvider::getFreetype(), path.c_str(), 0, &fontFace_)) {
        SPDLOG_ERROR("FT_New_Face Error, code {}", err);
        throw;
    }

    if (FT_Error err = FT_Select_Charmap(fontFace_, FT_ENCODING_UNICODE)) {
        SPDLOG_ERROR("FT_Select_Charmap Error, code {}", err);
        FT_Done_Face(fontFace_);
        throw;
    }

    if (FT_Error err = FT_Set_Pixel_Sizes(fontFace_, 0, size)) { // TODO: Replace with Set_Char_Size?
        SPDLOG_ERROR("FT_Set_Pixel_Sizes Error, code {}", err);
        FT_Done_Face(fontFace_);
        throw;
    }

    shader_ = std::make_shared<Shader>("res/text.vert", "res/text.frag");
    shader_->use();
    shader_->set_int_uniform("textureAtlas", 1);
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
}

eos::Font::~Font() {
    FT_Done_Face(fontFace_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void eos::Font::cache(const std::string& chars) {
    std::u32string utf32 = utf8::utf8to32(chars);

    for (size_t i = 0; i < utf32.length(); ++i) {
        cache(chars[i], utf32[i]);
    }
}

void eos::Font::cache(const std::basic_string<char8_t>& chars) {
    std::vector<uint32_t> utf32;
    utf8::utf8to32(chars.begin(), chars.end(), std::back_inserter(utf32));

    for (size_t i = 0; i < utf32.size(); ++i) {
        cache(chars[i], utf32[i]);
    }
}

void eos::Font::cache(char c, uint32_t codepoint) {
    if (auto it = glyphMap_.find(c); it != glyphMap_.end()) {
        // already cached
    } else {
        FT_UInt glyphIndex = FT_Get_Char_Index(fontFace_, codepoint);
        if (glyphIndex == 0) {
            SPDLOG_WARN("Character {} index is 0. Will not render.", codepoint);
        }

        if (FT_Error err = FT_Load_Glyph(fontFace_, glyphIndex, FT_LOAD_RENDER)) { // LOAD_TARGET_LCD?
            SPDLOG_ERROR("FT_Load_Char Error, code {}", err);
            FT_Done_Face(fontFace_);
            return;
        }

        /*
        if(FT_Error err = FT_Render_Glyph(fontFace_->glyph, FT_RENDER_MODE_NORMAL)){
            SPDLOG_ERROR("FT_Render_Glyph Error, code {}", err);
            FT_Done_Face(fontFace_);
            return;
        }
         */

        FT_GlyphSlot& glyph = fontFace_->glyph;
        FT_Bitmap& bitmap = glyph->bitmap;
        SPDLOG_TRACE("Caching {} ({}) {}x{}", c, codepoint, bitmap.width, bitmap.rows);
        size_t index = textureAtlas_.insert(bitmap.buffer, {bitmap.width, bitmap.rows});
        glyphMap_.emplace(codepoint, Glyph{index, {glyph->bitmap_left, glyph->bitmap_top}, glyph->advance.x});
    }
}

void eos::Font::render(const std::string& text, glm::vec2 pos, eos::ColorRGB color) const {
    std::vector<Vertices> vertices(text.length());
    std::vector<Indices> indices(text.length());

    std::u32string utf32 = setup_render(text);
    for (size_t i = 0; i < utf32.length(); ++i) {
        Glyph glyph = glyphMap_.at(utf32[i]);
        const eos::IrregularTextureAtlas::SubTexture& character = textureAtlas_[glyph.index];

        vertices[i].bl = {
                {
                        pos.x + glyph.bearing.x,
                        pos.y - character.dim.y + glyph.bearing.y,
                        character.texture.x,
                        character.texture.y + character.texture.w
                },
                color
        };
        vertices[i].br = {
                {
                        pos.x + glyph.bearing.x + character.dim.x,
                        pos.y - character.dim.y + glyph.bearing.y,
                        character.texture.x + character.texture.z,
                        character.texture.y + character.texture.w
                },
                color
        };
        vertices[i].tr = {
                {
                        pos.x + glyph.bearing.x + character.dim.x,
                        pos.y + glyph.bearing.y,
                        character.texture.x + character.texture.z,
                        character.texture.y
                },
                color
        };
        vertices[i].tl = {
                {
                        pos.x + glyph.bearing.x,
                        pos.y + glyph.bearing.y,
                        character.texture.x,
                        character.texture.y
                },
                color
        };

        indices[i].bl1 = i * 4 + 0;
        indices[i].br1 = i * 4 + 1;
        indices[i].tr1 = i * 4 + 2;
        indices[i].bl2 = i * 4 + 0;
        indices[i].tr2 = i * 4 + 2;
        indices[i].tl2 = i * 4 + 3;

        pos.x += glyph.advance >> 6;
    }

    glBufferData(GL_ARRAY_BUFFER, text.length() * sizeof(Vertices), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, text.length() * sizeof(Indices), &indices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_SHORT, nullptr);
}

void eos::Font::render(const std::string& text, glm::vec2 pos, eos::ColorHSV gradientStartColor,
                       eos::ColorHSV gradientStopColor) const {
    std::vector<Vertices> vertices(text.length());
    std::vector<Indices> indices(text.length());

    ColorRGB leftColor = gradientStartColor;
    ColorHSV colorStep = (gradientStopColor - gradientStartColor) / text.length();

    std::u32string utf32 = setup_render(text);
    for (size_t i = 0; i < utf32.length(); ++i) {
        Glyph glyph = glyphMap_.at(utf32[i]);
        const eos::IrregularTextureAtlas::SubTexture& character = textureAtlas_[glyph.index];

        gradientStartColor += colorStep;
        ColorRGB rightColor = gradientStartColor;

        vertices[i].bl = {
                {
                        pos.x + glyph.bearing.x,
                        pos.y - character.dim.y + glyph.bearing.y,
                        character.texture.x,
                        character.texture.y + character.texture.w
                },
                leftColor
        };
        vertices[i].br = {
                {
                        pos.x + glyph.bearing.x + character.dim.x,
                        pos.y - character.dim.y + glyph.bearing.y,
                        character.texture.x + character.texture.z,
                        character.texture.y + character.texture.w
                },
                rightColor
        };
        vertices[i].tr = {
                {
                        pos.x + glyph.bearing.x + character.dim.x,
                        pos.y + glyph.bearing.y,
                        character.texture.x + character.texture.z,
                        character.texture.y
                },
                rightColor
        };
        vertices[i].tl = {
                {
                        pos.x + glyph.bearing.x,
                        pos.y + glyph.bearing.y,
                        character.texture.x,
                        character.texture.y
                },
                leftColor
        };

        indices[i].bl1 = i * 4 + 0;
        indices[i].br1 = i * 4 + 1;
        indices[i].tr1 = i * 4 + 2;
        indices[i].bl2 = i * 4 + 0;
        indices[i].tr2 = i * 4 + 2;
        indices[i].tl2 = i * 4 + 3;

        pos.x += glyph.advance >> 6;
        leftColor = rightColor;
    }

    glBufferData(GL_ARRAY_BUFFER, text.length() * sizeof(Vertices), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, text.length() * sizeof(Indices), &indices[0], GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_SHORT, nullptr);
}

void eos::Font::render_cache(glm::vec2 pos, glm::vec2 scale) {
#ifdef DEBUG
    textureAtlas_.render(pos, scale);
#endif //DEBUG
}

std::u32string eos::Font::setup_render(const std::string& text) const {
    shader_->use();
    glm::vec2 windowDim = eos::ServiceProvider::getWindow().get_size();
    glm::mat4 projection = glm::ortho(0.0f, windowDim.x, 0.0f, windowDim.y);
    shader_->set_mat4_uniform("projection", projection);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, textureAtlas_);
    glBindSampler(1, 0);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 2, reinterpret_cast<const void*>(sizeof(glm::vec4)));

    return utf8::utf8to32(text);
}