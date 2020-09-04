//
// Created by jakob on 29.08.20.
//

#ifndef EOS_IRREGULARTEXTUREATLAS_HPP
#define EOS_IRREGULARTEXTUREATLAS_HPP

#include <map>
#include "RegularTextureAtlas.hpp"

namespace eos {
    class IrregularTextureAtlas : public TextureAtlas {
    public:
        struct SubTexture {
            glm::uvec2 dim; // Dimensions of the texture in integer pixels
            glm::vec4 texture; // Position and dimensions of the texture in float texture coordinates
        };

        explicit IrregularTextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat = -1);

        size_t insert(const void* data, glm::uvec2 textureDim);

        const SubTexture& operator[](size_t slot) const;

        glm::uvec2 get_free_slot(glm::uvec2 textureDim);

    private:
        //const uint minLineHeight = 10;
        const float minTexHeightMultiplier = 0.5;
        const float newLineHeightMultiplier = 1.1;

        std::vector<SubTexture> slots_;

        std::map<unsigned int, glm::uvec2> lines_; // <lineHeight, {xEmptyGlyphSlot, yEmptyGlyphSlot}>
    };
}

#endif //EOS_IRREGULARTEXTUREATLAS_HPP
