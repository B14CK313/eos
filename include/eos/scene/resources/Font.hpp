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
#include <eos/core/Color.hpp>
#include <freetype/freetype.h>
#include "IrregularTextureAtlas.hpp"

namespace eos {
    class Font {
    public:
        struct Glyph {
            size_t index;
            glm::uvec2 bearing;
            long advance;
        };

        Font(std::string_view path, float size);

        ~Font();

        void cache(const std::string& chars);

        void cache(const std::basic_string<char8_t>& chars);

        void cache(char c, uint32_t codepoint);

        void render(const std::string& text, glm::vec2 pos, eos::Color color = 0x33_l, glm::vec2 scale = {1.0f, 1.0f});

        void render_cache(glm::vec2 pos, glm::vec2 scale);

    private:
        unsigned int vao_;
        unsigned int vbo_;
        unsigned int ebo_;

        std::string_view path_;

        FT_Face fontFace_;
        IrregularTextureAtlas textureAtlas_{{512, 512}, GL_UNSIGNED_BYTE, GL_RED};

        std::unordered_map<char, Glyph> glyphMap_;

        std::shared_ptr<Shader> shader_;
    };
}

#endif //EOS_FONT_HPP
