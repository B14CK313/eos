//
// Created by jakob on 05.09.20.
//

#pragma once

#include "Font.hpp"

namespace eos {
    class Text {
    public:
        explicit Text(std::shared_ptr<Font> font);

        ~Text();

        void render(const std::string& text, glm::vec2 pos, eos::ColorRGB color = 0x00_l) const;

    protected:
        unsigned int vao_;
        unsigned int vbo_;
        unsigned int ebo_;

        std::shared_ptr<Font> font_;
        std::shared_ptr<Shader> shader_;

    protected:
        Text(std::shared_ptr<Font> font, std::shared_ptr<Shader> shader);

        std::u32string setup_render(const std::string& text) const;
    };
}
