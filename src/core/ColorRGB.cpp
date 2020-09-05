//
// Created by jakob on 03.08.20.
//

#include <string>
#include <spdlog/spdlog.h>
#include "eos/core/ColorHSV.hpp"
#include "eos/core/ColorRGB.hpp"

eos::ColorRGB::ColorRGB(float red, float green, float blue, float alpha) : red_(red), green_(green), blue_(blue), alpha_(alpha) {}

eos::ColorRGB::ColorRGB(unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha) : red_(red / 255.0f), green_(green / 255.0f), blue_(blue / 255.0f), alpha_(alpha / 255.0f) {}

eos::ColorRGB::ColorRGB(const std::string& hex) {
    unsigned int hexNum = std::stoi(hex.c_str(), nullptr, 16);
    if (hex.length() == 2) {
        float value = hexNum / 255.0f;
        red_ = value;
        green_ = value;
        blue_ = value;
        alpha_ = 1.0f;
    } else if (hex.length() == 3) {
        red_ = ((hexNum >> 8u) & 0xFu) / 15.0f;
        green_ = ((hexNum >> 4u) & 0xFu) / 15.0f;
        blue_ = ((hexNum) & 0xFu) / 15.0f;
        alpha_ = 1.0f;
    } else if (hex.length() == 4) {
        red_ = ((hexNum >> 12u) & 0xFu) / 15.0f;
        green_ = ((hexNum >> 8u) & 0xFu) / 15.0f;
        blue_ = ((hexNum >> 4u) & 0xFu) / 15.0f;
        alpha_ = ((hexNum) & 0xFu) / 15.0f;
    } else if (hex.length() == 6) {
        red_ = ((hexNum >> 16u) & 0xFFu) / 255.0f;
        green_ = ((hexNum >> 8u) & 0xFFu) / 255.0f;
        blue_ = ((hexNum) & 0xFFu) / 255.0f;
        alpha_ = 1.0f;
    } else if (hex.length() == 8) {
        red_ = ((hexNum >> 24u) & 0xFFu) / 255.0f;
        green_ = ((hexNum >> 16u) & 0xFFu) / 255.0f;
        blue_ = ((hexNum >> 8u) & 0xFFu) / 255.0f;
        alpha_ = ((hexNum) & 0xFFu) / 255.0f;
    } else {
        SPDLOG_WARN("Hex value {} has no known format", hex);
    }
}

eos::ColorHSV eos::ColorRGB::to_hsv() const {
    float max = std::max({red_, green_, blue_});
    float min = std::min({red_, green_, blue_});
    float delta = max - min;

    ColorHSV hsv{0, 0.0f, max, alpha_};
    if(max > 0.0f && delta > 0.0f) {
        hsv.saturation_ = delta / max;

        if(max == red_){
            hsv.hue_ = 60 * ((green_ - blue_) / delta);
        } else if(max == green_) {
            hsv.hue_ = 60 * ((blue_ - red_) / delta + 2);
        } else if(max == blue_){
            hsv.hue_ = 60 * ((red_ - green_) / delta + 4);
        }
        if(hsv.hue_ < 0) hsv.hue_ += 360;
    }
    return hsv;
}

eos::ColorRGB::operator ColorHSV() const {
    return to_hsv();
}

eos::ColorRGB::operator glm::vec4() const {
    return glm::vec4(red_, green_, blue_, alpha_);
}

eos::ColorRGB::operator glm::vec3() const {
    return glm::vec3(red_, green_, blue_);
}

eos::ColorRGB operator ""_l(unsigned long long int hex) {
    float value = hex / 255.0f;
    return eos::ColorRGB(value, value, value, 1.0f);
}

eos::ColorRGB operator ""_srgb(unsigned long long int hex) {
    return eos::ColorRGB(((hex >> 8u) & 0xFu) / 15.0f,
                      ((hex >> 4u) & 0xFu) / 15.0f,
                      (hex & 0xFu) / 15.0f,
                         1.0f);
}

eos::ColorRGB operator ""_srgba(unsigned long long int hex) {
    return eos::ColorRGB(((hex >> 12u) & 0xFu) / 15.0f,
                      ((hex >> 8u) & 0xFu) / 15.0f,
                      ((hex >> 4u) & 0xFu) / 15.0f,
                      (hex & 0xFu) / 15.0f);
}

eos::ColorRGB operator ""_rgb(unsigned long long int hex) {
    return eos::ColorRGB(((hex >> 16u) & 0xFFu) / 255.0f,
                      ((hex >> 8u) & 0xFFu) / 255.0f,
                      ((hex) & 0xFFu) / 255.0f,
                         1.0f);
}

eos::ColorRGB operator ""_rgba(unsigned long long int hex) {
    return eos::ColorRGB(((hex >>= 8u) & 0xFFu) / 255.0f,
                      ((hex >>= 8u) & 0xFFu) / 255.0f,
                      ((hex >> 8u) & 0xFFu) / 255.0f,
                      ((hex) & 0xFFu) / 255.0f);
}
