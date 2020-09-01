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
#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#include "IrregularTextureAtlas.hpp"

namespace eos {
    class Font {
    public:
        Font(const std::string_view path, float size);

        void cache(std::string_view chars);

        void cache(char c, uint32_t codepoint);

        void render(std::string_view text, glm::vec2 pos);

    private:
        const int dpi_ = 72;
        const int horRes_ = 64;

        std::shared_ptr<Shader> shader_;

        std::string_view path_;
        msdfgen::FontHandle* font_;
        IrregularTextureAtlas textureAtlas_{{512, 512}, GL_FLOAT, GL_RGB};

        std::unordered_map<char, size_t> glyphMap_; // char => index in TextureAtlas;
    };
}

#endif //EOS_FONT_HPP
