//
// Created by jakob on 29.08.20.
//

#ifndef EOS_REGULARTEXTUREATLAS_HPP
#define EOS_REGULARTEXTUREATLAS_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <spdlog/spdlog.h>
#include "Shader.h"
#include "TextureAtlas.hpp"

namespace eos {
    class RegularTextureAtlas : public TextureAtlas {
    public:
        explicit RegularTextureAtlas(glm::uvec2 atlasDim, glm::uvec2 textureDim, int type, int format,
                                     int internalformat = -1);

        size_t insert(const void* data);

        glm::uvec4 operator[](size_t slot) const override;

        glm::uvec2 get_free_slot();

    protected:
        glm::uvec2 textureDim_;

        std::vector<glm::uvec2> slots_;

        unsigned int slotsPerRow_;
    };
}

#endif //EOS_REGULARTEXTUREATLAS_HPP
