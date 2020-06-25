//
// Created by jakob on 25.06.20.
//

#include <glad/glad.h>
#include <mango/image/image.hpp>
#include <spdlog/spdlog.h>
#include <eos/utils.hpp>
#include <filesystem>
#include "eos/Texture.h"

eos::Texture::Texture(const std::string& path, unsigned int colorFormat, unsigned int wrapS, unsigned int wrapT,
                 unsigned int filterMin, unsigned int filterMag) {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    set_wrap(wrapS, wrapT);
    set_filter(filterMin, filterMag);

    std::vector<unsigned char> buffer = eos::utils::load_file_unsigned_char(path);
    mango::Memory memory(buffer.data(), buffer.size());
    mango::ImageDecoder decoder(memory, std::filesystem::path(path).extension());
    if(decoder.isDecoder()){
        mango::ImageHeader header = decoder.header();
        const int stride = header.width * header.format.bytes();
        mango::Surface surface(header.width, header.height, header.format, stride, &id_);
        decoder.decode(surface);
    }
}

void eos::Texture::bind() const {
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
