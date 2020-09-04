//, textureDim_.x, textureDim_.y
// Created by jakob on 29.08.20.
//

#include "eos/scene/resources/RegularTextureAtlas.hpp"

eos::RegularTextureAtlas::RegularTextureAtlas(glm::uvec2 atlasDim, glm::uvec2 textureDim, int type, int format,
                                              int internalformat)
        : TextureAtlas(atlasDim, type, format, internalformat), textureDim_(textureDim) {
    slotsPerRow_ = atlasDim.x / textureDim.x;
}

size_t eos::RegularTextureAtlas::insert(const void* data) {
    glm::uvec2 slot = get_free_slot();

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, slot.x, slot.y, textureDim_.x, textureDim_.y, format_, type_, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    slots_.push_back(slot);

    return slots_.size() - 1;
}

glm::uvec2 eos::RegularTextureAtlas::operator[](size_t slot) const {
    if (slot < slots_.size()) {
        return slots_[slot], slots_[slot];
    } else {
        throw;
    }
}

glm::uvec2 eos::RegularTextureAtlas::get_free_slot() {
    glm::uvec4 slot{(slots_.size() % slotsPerRow_) * textureDim_.x, (slots_.size() / slotsPerRow_) * textureDim_.y, textureDim_.x, textureDim_.y};
    if (slot.y + textureDim_.y <= atlasDim_.y) {
        return slot;
    } else {
        SPDLOG_ERROR("TextureAtlas cache overflow!");
        return atlasDim_;
    }
}
