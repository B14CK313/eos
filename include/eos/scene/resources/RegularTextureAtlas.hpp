//
// Created by jakob on 29.08.20.
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <spdlog/spdlog.h>
#include "Shader.hpp"
#include "TextureAtlas.hpp"

namespace eos {
    class RegularTextureAtlas : public TextureAtlas {
    public:
        explicit RegularTextureAtlas(glm::uvec2 atlasDim, glm::uvec2 textureDim, int type, int format,
                                     int internalformat = -1);

        size_t insert(const void* data);

        glm::uvec2 operator[](size_t slot) const;

        glm::uvec2 get_free_slot();

    protected:
        glm::uvec2 textureDim_;

        std::vector<glm::uvec2> slots_;

        unsigned int slotsPerRow_;
    };
}
