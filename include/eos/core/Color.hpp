//
// Created by jakob on 03.08.20.
//

#ifndef EOS_COLOR_HPP
#define EOS_COLOR_HPP

#include <glm/glm.hpp>
#include <string>

struct uint12_t {
    explicit uint12_t(uint16_t i);

    operator uint16_t() const;

    uint16_t b : 12;
};

struct uint24_t {
    explicit uint24_t(uint32_t i);

    operator uint32_t() const;

    uint32_t b : 24;
};

namespace eos {

    class Color {
    public:
        union {
            struct {
                float r_;
                float g_;
                float b_;
                float a_;
            };
            float compound_[4];
        };

        Color();

        /**
         *
         * @param hex An int in grayscale format (e.g. 0x33)
         */
        explicit Color(uint8_t hex);

        /**
         *
         * @param hex An int in shorthand RGB format (e.g. 0x369)
         */
        explicit Color(uint12_t hex);

        /**
         *
         * @param hex An int in shorthand RGBA format (e.g. 0x369F)
         */
        explicit Color(uint16_t hex);

        /**
         *
         * @param hex An int in RGB format (e.g. 0x336699)
         */
        explicit Color(uint24_t hex);

        /**
         *
         * @param hex An int in RGBA format (e.g. 0x336699FF)
         */
        explicit Color(uint32_t hex);

        explicit Color(std::string_view hex);

        Color(float r, float g, float b, float a = 1.0f);

        Color(short r, short g, short b, short a = 255);

        operator glm::vec4() const;

        operator glm::vec3() const;
    };
}

#endif //EOS_COLOR_HPP
