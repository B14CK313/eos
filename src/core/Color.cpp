//
// Created by jakob on 03.08.20.
//

#include <string>
#include <spdlog/spdlog.h>
#include "eos/core/Color.hpp"

eos::Color::Color() {}

eos::Color::Color(float r, float g, float b, float a) : compound_{r, g, b, a} {}

eos::Color::Color(short r, short g, short b, short a) : compound_{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f} {}

eos::Color::Color(const std::string& hex) {
    unsigned int hexNum = std::stoi(hex.c_str(), nullptr, 16);
    if (hex.length() == 2) {
        float value = hexNum / 255.0f;
        r_ = value;
        g_ = value;
        b_ = value;
        a_ = 1.0f;
    } else if (hex.length() == 3) {
        r_ = ((hexNum >> 8u) & 0xFu) / 15.0f;
        g_ = ((hexNum >> 4u) & 0xFu) / 15.0f;
        b_ = ((hexNum) & 0xFu) / 15.0f;
        a_ = 1.0f;
    } else if (hex.length() == 4) {
        r_ = ((hexNum >> 12u) & 0xFu) / 15.0f;
        g_ = ((hexNum >> 8u) & 0xFu) / 15.0f;
        b_ = ((hexNum >> 4u) & 0xFu) / 15.0f;
        a_ = ((hexNum) & 0xFu) / 15.0f;
    } else if (hex.length() == 6) {
        r_ = ((hexNum >> 16u) & 0xFFu) / 255.0f;
        g_ = ((hexNum >> 8u) & 0xFFu) / 255.0f;
        b_ = ((hexNum) & 0xFFu) / 255.0f;
        a_ = 1.0f;
    } else if (hex.length() == 8) {
        r_ = ((hexNum >> 24u) & 0xFFu) / 255.0f;
        g_ = ((hexNum >> 16u) & 0xFFu) / 255.0f;
        b_ = ((hexNum >> 8u) & 0xFFu) / 255.0f;
        a_ = ((hexNum) & 0xFFu) / 255.0f;
    } else {
        SPDLOG_WARN("Hex value {} has no known format", hex);
    }
}

eos::Color::operator glm::vec4() const {
    return glm::vec4(r_, g_, b_, a_);
}

eos::Color::operator glm::vec3() const {
    return glm::vec3(r_, g_, b_);
}

eos::Color operator ""_l(unsigned long long int hex) {
    float value = hex / 255.0f;
    return eos::Color(value, value, value, 1.0f);
}

eos::Color operator ""_srgb(unsigned long long int hex) {
    return eos::Color(((hex >> 8u) & 0xFu) / 15.0f,
                      ((hex >> 4u) & 0xFu) / 15.0f,
                      (hex & 0xFu) / 15.0f,
                      1.0f);
}

eos::Color operator ""_srgba(unsigned long long int hex) {
    return eos::Color(((hex >> 12u) & 0xFu) / 15.0f,
                      ((hex >> 8u) & 0xFu) / 15.0f,
                      ((hex >> 4u) & 0xFu) / 15.0f,
                      (hex & 0xFu) / 15.0f);
}

eos::Color operator ""_rgb(unsigned long long int hex) {
    return eos::Color(((hex >> 16u) & 0xFFu) / 255.0f,
                      ((hex >> 8u) & 0xFFu) / 255.0f,
                      ((hex) & 0xFFu) / 255.0f,
                      1.0f);
}

eos::Color operator ""_rgba(unsigned long long int hex) {
    return eos::Color(((hex >>= 8u) & 0xFFu) / 255.0f,
                      ((hex >>= 8u) & 0xFFu) / 255.0f,
                      ((hex >> 8u) & 0xFFu) / 255.0f,
                      ((hex) & 0xFFu) / 255.0f);
}
