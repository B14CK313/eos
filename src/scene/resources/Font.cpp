//
// Created by jakob on 26.08.20.
//

#include <spdlog/spdlog.h>
#include <eos/core/ServiceProvider.h>
#include <utf8/checked.h>
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
}

eos::Font::~Font() {
    FT_Done_Face(fontFace_);
}

void eos::Font::cache(const std::string& chars) {
    std::u32string utf32 = utf8::utf8to32(chars);

    for (auto codepoint : utf32) {
        cache(codepoint);
    }
}

void eos::Font::cache(const std::basic_string<char8_t>& chars) {
    std::vector<uint32_t> utf32;
    utf8::utf8to32(chars.begin(), chars.end(), std::back_inserter(utf32));

    for (auto codepoint : utf32) {
        cache(codepoint);
    }
}

void eos::Font::cache(char32_t codepoint) {
    if (auto it = characterMap_.find(codepoint); it != characterMap_.end()) {
        // already cached
    } else {
        FT_UInt glyphIndex = FT_Get_Char_Index(fontFace_, codepoint);
        if (glyphIndex == 0) {
            SPDLOG_WARN("Character {} index is 0. Will not render.", static_cast<int32_t>(codepoint));
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
        characterMap_.emplace(codepoint, Character{textureAtlas_.insert(bitmap.buffer, {bitmap.width, bitmap.rows}), {glyph->bitmap_left, glyph->bitmap_top}, glyph->advance.x});
    }
}

eos::Font::Character eos::Font::get(char32_t codepoint) {
    if (auto it = characterMap_.find(codepoint); it != characterMap_.end()) {
        return characterMap_.at(codepoint);
    } else {
        SPDLOG_ERROR("Character {} not in cache!", static_cast<int32_t>(codepoint));
        return Character();
    }
}

void eos::Font::use() {
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, textureAtlas_);
    glBindSampler(1, 0);
}

void eos::Font::render(glm::vec2 pos, glm::vec2 scale) {
#ifdef DEBUG
    textureAtlas_.render(pos, scale);
#endif //DEBUG
}
