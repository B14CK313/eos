//
// Created by jakob on 30.08.20.
//

#ifndef EOS_TEXTUREATLAS_HPP
#define EOS_TEXTUREATLAS_HPP

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.h"

namespace eos {
    class TextureAtlas {
    public:
        virtual glm::uvec4 operator[](size_t slot) const = 0;

        GLuint texture_;

#ifdef DEBUG
        void render(glm::uvec2 pos, float scale) const;
#endif //DEBUG

    protected:
        TextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat = -1);

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
