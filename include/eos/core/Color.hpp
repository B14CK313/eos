//
// Created by jakob on 03.08.20.
//

#ifndef EOS_COLOR_HPP
#define EOS_COLOR_HPP

#include <glm/glm.hpp>
#include <string>

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

        Color(float r, float g, float b, float a = 1.0f);

        Color(short r, short g, short b, short a = 255);

        explicit Color(const std::string& hex);

        operator glm::vec4() const;

        operator glm::vec3() const;
    };
}

/**
 *
 * @param hex An int in grayscale format (e.g. 0x33)
 */
eos::Color operator ""_l(unsigned long long hex);

/**
 *
 * @param hex An int in shorthand RGB format (e.g. 0x369)
 */
eos::Color operator ""_srgb(unsigned long long hex);

/**
 *
 * @param hex An int in shorthand RGBA format (e.g. 0x369F)
 */
eos::Color operator ""_srgba(unsigned long long hex);

/**
 *
 * @param hex An int in RGB format (e.g. 0x336699)
 */
eos::Color operator ""_rgb(unsigned long long hex);

/**
 *
 * @param hex An int in RGBA format (e.g. 0x336699FF)
 */
eos::Color operator ""_rgba(unsigned long long hex);

#endif //EOS_COLOR_HPP
