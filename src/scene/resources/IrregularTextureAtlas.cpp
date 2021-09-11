//
// Created by jakob on 30.08.20.
//

#include <eos/scene/resources/IrregularTextureAtlas.hpp>

eos::IrregularTextureAtlas::IrregularTextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat)
        : TextureAtlas(atlasDim, type, format, internalformat) {}

eos::IrregularTextureAtlas::SubTexture eos::IrregularTextureAtlas::insert(const void* data, glm::uvec2 textureDim) {
    glm::vec2 slot = get_free_slot(textureDim);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, slot.x, slot.y, textureDim.x, textureDim.y, format_, type_, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return {textureDim, glm::vec4{slot.x / atlasDim_.x, slot.y / atlasDim_.y, static_cast<float>(textureDim.x) / atlasDim_.x, static_cast<float>(textureDim.y) / atlasDim_.y}};
}

glm::uvec2 eos::IrregularTextureAtlas::get_free_slot(glm::uvec2 textureDim) {
	for (auto& [lineHeight, slotCoords] : lines_) {
        if (textureDim.y < lineHeight) { // if texture height fits in line
			if(textureDim.y < lineHeight * minTexHeightMultiplier) { // texture too small for line -> create new line
                SPDLOG_TRACE("Texture {}x{} too small for line (min {}px) at ({}/{})", textureDim.x, textureDim.y, lineHeight * minTexHeightMultiplier, slotCoords.x, slotCoords.y);
                break;
            } else if(slotCoords.x + textureDim.x <= atlasDim_.x){ // if texture width fits in line
                SPDLOG_TRACE("Texture {}⨉{} fits into {}⨉{} at ({}/{})", textureDim.x, textureDim.y, atlasDim_.x - slotCoords.x, lineHeight, slotCoords.x, slotCoords.y);
                glm::uvec2 slot{slotCoords.x, slotCoords.y};
                slotCoords.x += textureDim.x;
                return slot;
            }
			SPDLOG_TRACE("Texture {}⨉{} doesn't fit into {}⨉{} at ({}/{})", textureDim.x, textureDim.y, atlasDim_.x - slotCoords.x, lineHeight, slotCoords.x, slotCoords.y);
        }
    }

    // no fitting line was found, create new line if texture fits into atlas
    unsigned int previousLineEnd{0};
    if(auto previousLine{lines_.rbegin()}; previousLine != lines_.rend()){
        previousLineEnd = previousLine->second.y + previousLine->first;
    }
    unsigned int availableSpace{atlasDim_.y - previousLineEnd};
    if(textureDim.y <= availableSpace){
        unsigned int lineHeight = std::min(static_cast<unsigned int>(std::ceil(textureDim.y * newLineHeightMultiplier)), availableSpace);
        SPDLOG_TRACE("Creating new {} high line at (0/{}) for Texture {}⨉{}", lineHeight, previousLineEnd, textureDim.x, textureDim.y);
        lines_.emplace(lineHeight, glm::uvec2{textureDim.x, previousLineEnd});
        return {0, previousLineEnd};
    } else { // texture does not fit in atlas
        SPDLOG_ERROR("TextureAtlas overflow: No new line could be created for Texture {}⨉{} (available space: {})!", textureDim.x, textureDim.y, availableSpace);
        return atlasDim_;
    }
}
