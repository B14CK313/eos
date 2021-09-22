//
// Created by jakob on 03.08.20.
//

#pragma once

#include <glm/glm.hpp>
#include <string>

namespace eos {
    class ColorHSV;

    class ColorRGB {
    public:
        float red_ = 0;
        float green_ = 0;
        float blue_ = 0;
        float alpha_ = 0;

    public:
        ColorRGB() = default;

        ColorRGB(float red, float green, float blue, float alpha = 1.0f);

        ColorRGB(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha = 255);

        explicit ColorRGB(const std::string& hex);

        ColorHSV to_hsv() const;

        operator ColorHSV() const;

        operator glm::vec4() const;

        operator glm::vec3() const;

        ColorRGB operator+(const ColorRGB& color) const;

        ColorRGB operator-(const ColorRGB& color) const;

        ColorRGB operator*(float scalar) const;

        ColorRGB operator/(float scalar) const;

        ColorRGB& operator+=(const ColorRGB& color);

        ColorRGB& operator-=(const ColorRGB& color);

        ColorRGB& operator*=(float scalar);

        ColorRGB& operator/=(float scalar);
    };
}

/**
 *
 * @param hex An int in grayscale format (e.g. 0x33)
 */
eos::ColorRGB operator ""_l(unsigned long long hex);

/**
 *
 * @param hex An int in shorthand RGB format (e.g. 0x369)
 */
eos::ColorRGB operator ""_srgb(unsigned long long hex);

/**
 *
 * @param hex An int in shorthand RGBA format (e.g. 0x369F)
 */
eos::ColorRGB operator ""_srgba(unsigned long long hex);

/**
 *
 * @param hex An int in RGB format (e.g. 0x336699)
 */
eos::ColorRGB operator ""_rgb(unsigned long long hex);

/**
 *
 * @param hex An int in RGBA format (e.g. 0x336699FF)
 */
eos::ColorRGB operator ""_rgba(unsigned long long hex);
