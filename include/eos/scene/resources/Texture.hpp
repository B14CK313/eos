//
// Created by jakob on 25.06.20.
//

#pragma once

#include <string>
#include <glad/glad.h>

namespace eos {

    class Texture {
    public:
        explicit Texture(const std::string& path, unsigned int colorFormat = GL_RGBA, unsigned int wrapS = GL_REPEAT,
                unsigned int wrapT = GL_REPEAT, unsigned int filterMin = GL_LINEAR_MIPMAP_LINEAR, unsigned int filterMag = GL_LINEAR_MIPMAP_LINEAR);

        virtual ~Texture();

        void bind() const;

        [[nodiscard]] unsigned int get_id() const;

        // set the texture wrapping parameters
        const Texture& set_wrap(unsigned int wrapS, unsigned int wrapT) const;

        // set texture filtering parameters
        const Texture& set_filter(unsigned int filterMin, unsigned int filterMag) const;

    private:
        unsigned int pboId_{};
        unsigned int textureId_{};
    };

}
