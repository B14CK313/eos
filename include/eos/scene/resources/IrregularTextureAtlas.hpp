//
// Created by jakob on 29.08.20.
//

#pragma once

#include <map>
#include "RegularTextureAtlas.hpp"

namespace eos {
    class IrregularTextureAtlas : public TextureAtlas {
    public:
        struct SubTexture {
            glm::uvec2 pixels; // Dimensions of the texture in integer pixels
            glm::vec4 coords; // Position and dimensions of the texture in float texture coordinates
        };

        explicit IrregularTextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat = -1);

        SubTexture insert(const void* data, glm::uvec2 textureDim);

        glm::uvec2 get_free_slot(glm::uvec2 textureDim);

    private:
        //const uint minLineHeight = 10;
        const float minTexHeightMultiplier = 0.5;
        const float newLineHeightMultiplier = 1.1;

        std::map<unsigned int, glm::uvec2> lines_; // <lineHeight, {xEmptyGlyphSlot, yEmptyGlyphSlot}>
    };
}
