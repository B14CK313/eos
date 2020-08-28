//
// Created by jakob on 27.08.20.
//

#ifndef EOS_TEXT_HPP
#define EOS_TEXT_HPP

#include <cstddef>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

namespace eos {
    class Text {
    public:
        Text(size_t length, std::shared_ptr<eos::Shader> shader, size_t textVAO, size_t textVBO, size_t textIBO, unsigned int cacheTexture);

        ~Text();

        void set_position(glm::vec3 pos);

        void render();

    private:
        size_t length_;
        std::shared_ptr<eos::Shader> shader_;
        unsigned int textVAO_;
        unsigned int textVBO_;
        unsigned int textIBO_;
        unsigned int cacheTexture_;
    };
}

#endif //EOS_TEXT_HPP
