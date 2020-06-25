//
// Created by jakob on 25.06.20.
//

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <spdlog/spdlog.h>
#include "eos/Texture.h"

eos::Texture::Texture(const std::string& path, unsigned int colorFormat, unsigned int wrapS, unsigned int wrapT,
                 unsigned int filterMin, unsigned int filterMag) {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    set_wrap(wrapS, wrapT);
    set_filter(filterMin, filterMag);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        SPDLOG_ERROR("Failed to load texture from {}", path);
    }
    stbi_image_free(data);
}

void eos::Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, id_);
}

void eos::Texture::cleanup() {
    glDeleteTextures(1, &id_);
}

const eos::Texture& eos::Texture::set_wrap(unsigned int wrapS, unsigned int wrapT) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    return *this;
}

const eos::Texture& eos::Texture::set_filter(unsigned int filterMin, unsigned int filterMag) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    return *this;
}
