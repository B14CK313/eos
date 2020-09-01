//
// Created by jakob on 30.08.20.
//

#include <eos/scene/resources/IrregularTextureAtlas.hpp>

eos::IrregularTextureAtlas::IrregularTextureAtlas(glm::uvec2 atlasDim, int type, int format, int internalformat)
        : TextureAtlas(atlasDim, type, format, internalformat) {}

size_t eos::IrregularTextureAtlas::insert(const void* data, glm::uvec2 textureDim) {
    glm::uvec4 slot = get_free_slot(textureDim);

    shader_->use();
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, slot.x, slot.y, textureDim.x, textureDim.y, format_, type_, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    slots_.push_back(slot);

    return slots_.size() - 1;
}

glm::uvec4 eos::IrregularTextureAtlas::operator[](size_t slot) const {
    if (slot < slots_.size()) {
        return slots_[slot];
    } else {
        throw;
    }
}

glm::uvec4 eos::IrregularTextureAtlas::get_free_slot(glm::uvec2 textureDim) {
    for (auto it = lines_.begin(); it != lines_.end(); ++it) {
        if (textureDim.y < it->first) { // if texture height fits in line
            SPDLOG_TRACE("Texture height ({}) fits into {} high line at {} ", textureDim.y, it->first, it->second.y);
            if(textureDim.y < it->first * minTexHeightMultiplier) { // texture too small for line -> create new line
                SPDLOG_TRACE("Texture too small for line (min {}) -> create new line", it->first * minTexHeightMultiplier);
                break;
            }
            if(it->second.x + textureDim.x <= atlasDim_.x){ // if texture width fits in line
                SPDLOG_TRACE("Texture width ({}) fits in {} in line {} high at ({}/{})", textureDim.x, atlasDim_.x - it->second.x, it->first, it->second.x, it->second.y);
                glm::uvec4 slot = {it->second.x, it->second.y, textureDim.x, textureDim.y};
                it->second.x += textureDim.x;
                return slot;
            } else {
                SPDLOG_TRACE("Texture width ({}) fits not in {} in line {} high at ({}/{})", textureDim.x, atlasDim_.x - it->second.x, it->first, it->second.x, it->second.y);
                continue;
            }
        } else continue;
    }

    // no fitting line was found, create new line if texture fits
    SPDLOG_TRACE("No fitting line was found");
    unsigned int previousLineEnd = 0;
    if(auto previousLine = lines_.rbegin(); previousLine != lines_.rend()){
        previousLineEnd = previousLine->second.y + previousLine->first;
    }
    unsigned int requiredSpace = previousLineEnd + textureDim.y;
    SPDLOG_TRACE("Texture ({}x{}) at {}: Requires space {} of {}", textureDim.x, textureDim.y, previousLineEnd, requiredSpace, atlasDim_.y);
    if(lines_.empty() || requiredSpace <= atlasDim_.y){
        unsigned int lineHeight = std::min(static_cast<unsigned int>(std::ceil(textureDim.y * newLineHeightMultiplier)), atlasDim_.y - requiredSpace);
        SPDLOG_TRACE("Creating new {} high line at {}", lineHeight, previousLineEnd);
        lines_.emplace(lineHeight, glm::uvec2{textureDim.x, previousLineEnd});
        return {0, previousLineEnd, textureDim.x, textureDim.y};
    } else { // texture does not fit in atlas
        SPDLOG_ERROR("TextureAtlas cache overflow!");
        return {0, 0, 0, 0};
    }
}
