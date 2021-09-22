//
// Created by jakob on 30.08.20.
//

#ifndef EOS_TEXTUREATLAS_HPP
#define EOS_TEXTUREATLAS_HPP

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"

namespace eos {
    class TextureAtlas {
    public:
        operator GLuint() const;

        void render(glm::uvec2 pos, glm::vec2 scale) const;

    protected:
        TextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat = -1);

        GLuint texture_;
        int type_;
        int format_;
        int internalformat_;
        glm::uvec2 atlasDim_;

#ifdef DEBUG
        std::shared_ptr<Shader> shader_;
        unsigned int vao_;
        unsigned int vbo_;
#endif //DEBUG
    };
}

#endif //EOS_TEXTUREATLAS_HPP
