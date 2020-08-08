//
// Created by jakob on 25.06.20.
//

#include <glad/glad.h>
#include <mango/image/image.hpp>

#include <spdlog/spdlog.h>
#include "eos/scene/resources/Texture.h"

eos::Texture::Texture(const std::string_view path, unsigned int colorFormat, unsigned int wrapS, unsigned int wrapT,
                      unsigned int filterMin, unsigned int filterMag) {
    // Creates memory mapped file
    mango::filesystem::File file(path.data());
    mango::ImageDecoder decoder(file, mango::filesystem::getExtension(path.data()));
    // Can decode file
    if (decoder.isDecoder()) {
        mango::ImageHeader header = decoder.header();
        const int stride = header.width * header.format.bytes();

        // Generate ids for texture and PBO and bind to them
        glGenTextures(1, &textureId_);
        glBindTexture(GL_TEXTURE_2D, textureId_);
        glGenBuffers(1, &pboId_);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId_);

        set_wrap(wrapS, wrapT);
        set_filter(filterMin, filterMag);

        // allocate memory on GPU
        glBufferData(GL_PIXEL_UNPACK_BUFFER, stride * header.height, nullptr, GL_STREAM_DRAW);
        void* mappedBuffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        if (mappedBuffer) {
            // define output
            mango::Surface surface(header.width, header.height, mango::FORMAT_R8G8B8A8, stride, mappedBuffer);
            SPDLOG_TRACE("Loading image; format: R: {}, G: {}, B: {}, A: {}, O: {} {} {} {}, type: {} is float: {}",
                        header.format.size[0], header.format.size[1], header.format.size[2], header.format.size[3],
                        header.format.offset[0], header.format.offset[1], header.format.offset[2],
                        header.format.offset[3], header.format.type, header.format.isFloat());
            SPDLOG_TRACE("To surface with format: R: {}, G: {}, B: {}, A: {}, O: {} {} {} {}, type: {} is float: {}",
                        surface.format.size[0], surface.format.size[1], surface.format.size[2], surface.format.size[3],
                        surface.format.offset[0], surface.format.offset[1], surface.format.offset[2],
                        surface.format.offset[3], surface.format.type, surface.format.isFloat());
            // decode directly into mapped memory
            mango::ImageDecodeStatus status = decoder.decode(surface);
            surface.yflip();

            if (!status.success) {
                SPDLOG_ERROR("Failed to decode image, {}", status.info);
            }

            if (!glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER)) {
                SPDLOG_ERROR("Failed to unmap GL_PIXEL_UNPACK_BUFFER");
            }
        } else {
            SPDLOG_ERROR("Failed to map GL_PIXEL_UNPACK_BUFFER");
        }

        // Create texture from PBO
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header.width, header.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);
        // Buffer is not used anymore, unbind
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
}

eos::Texture::~Texture() {
    glDeleteBuffers(1, &pboId_);
    glDeleteTextures(1, &textureId_);}

void eos::Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, textureId_);
}

unsigned int eos::Texture::get_id() const {
    return textureId_;
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
