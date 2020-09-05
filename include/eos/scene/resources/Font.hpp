//
// Created by jakob on 26.08.20.
//

#ifndef EOS_FONT_HPP
#define EOS_FONT_HPP

/*
 * Glyph metrics:
 * --------------
 *
 *                       xmin                     xmax
 *                        |                         |
 *                        |<-------- width -------->|
 *                        |                         |
 *              |         +-------------------------+----------------- ymax
 *              |         |    ggggggggg   ggggg    |     ^        ^
 *              |         |   g:::::::::ggg::::g    |     |        |
 *              |         |  g:::::::::::::::::g    |     |        |
 *              |         | g::::::ggggg::::::gg    |     |        |
 *              |         | g:::::g     g:::::g     |     |        |
 *    offset_x -|-------->| g:::::g     g:::::g     |  offset_y    |
 *              |         | g:::::g     g:::::g     |     |        |
 *              |         | g::::::g    g:::::g     |     |        |
 *              |         | g:::::::ggggg:::::g     |     |        |
 *              |         |  g::::::::::::::::g     |     |      height
 *              |         |   gg::::::::::::::g     |     |        |
 *  baseline ---*---------|---- gggggggg::::::g-----*--------      |
 *            / |         |             g:::::g     |              |
 *     origin   |         | gggggg      g:::::g     |              |
 *              |         | g:::::gg   gg:::::g     |              |
 *              |         |  g::::::ggg:::::::g     |              |
 *              |         |   gg:::::::::::::g      |              |
 *              |         |     ggg::::::ggg        |              |
 *              |         |         gggggg          |              v
 *              |         +-------------------------+----------------- ymin
 *              |                                   |
 *              |------------- advance_x ---------->|
 */

#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <eos/core/ColorRGB.hpp>
#include "IrregularTextureAtlas.hpp"

namespace eos {
    class Font {
    public:
        struct Character {
            IrregularTextureAtlas::SubTexture texture;
            glm::uvec2 bearing;
            long advance;
        };

    public:
        Font(const std::string& path, float size);

        ~Font();

        void cache(const std::string& chars);

        void cache(const std::basic_string<char8_t>& chars);

        void cache(char32_t codepoint);

        Character get(char32_t codepoint);

        void use();

        void render(glm::vec2 pos, glm::vec2 scale);

    private:
        const std::string& path_;

        FT_Face fontFace_;

        IrregularTextureAtlas textureAtlas_{{512, 512}, GL_UNSIGNED_BYTE, GL_RED};

        std::unordered_map<char32_t, Character> characterMap_;
    };
}

#endif //EOS_FONT_HPP
