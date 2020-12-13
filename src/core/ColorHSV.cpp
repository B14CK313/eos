//
// Created by jakob on 04.09.20.
//

#include "eos/core/ColorRGB.hpp"
#include "eos/core/ColorHSV.hpp"

eos::ColorHSV::ColorHSV(float hue, float saturation, float value, float alpha) : hue_(hue),
                                                                                          saturation_(saturation),
                                                                                          value_(value),
                                                                                          alpha_(alpha) {}

eos::ColorRGB eos::ColorHSV::to_rgb() const {
    float h = hue_ >= 360 ? 0.0f : hue_ / 60.0f;
    int i = h;
    float frac = h - i;
    float p = value_ * (1 - saturation_);
    float q = value_ * (1 - saturation_ * frac);
    float t = value_ * (1 - saturation_ * (1 - frac));

    switch (i) {
        case 0:
            return {value_, t, p, alpha_};
        case 1:
            return {q, value_, p, alpha_};
        case 2:
            return {p, value_, t, alpha_};
        case 3:
            return {p, q, value_, alpha_};
        case 4:
            return {t, p, value_, alpha_};
        case 5:
            return {value_, p, q, alpha_};
        default:
            return {};
    }
}

eos::ColorHSV::operator ColorRGB() const {
    return to_rgb();
}

eos::ColorHSV eos::ColorHSV::operator+(const eos::ColorHSV& color) const {
    return {std::fmod(hue_ + color.hue_, 360.0f), saturation_ + color.saturation_,
            value_ + color.value_, alpha_ + color.alpha_};
}

eos::ColorHSV eos::ColorHSV::operator-(const eos::ColorHSV& color) const {
    return {std::fmod(hue_ - color.hue_, 360.0f), saturation_ - color.saturation_,
            value_ - color.value_, alpha_ - color.alpha_};
}

eos::ColorHSV eos::ColorHSV::operator*(float scalar) const {
    return {std::fmod(hue_ * scalar, 360.0f), saturation_ * scalar, value_ * scalar, alpha_ * scalar};
}

eos::ColorHSV eos::ColorHSV::operator/(float scalar) const {
    return {std::fmod(hue_ / scalar, 360.0f), saturation_ / scalar, value_ / scalar, alpha_ / scalar};
}

eos::ColorHSV& eos::ColorHSV::operator+=(const eos::ColorHSV& color) {
    hue_ = std::fmod(hue_ + color.hue_, 360.0f);
    saturation_ += color.saturation_;
    value_ += color.value_;
    alpha_ += color.alpha_;
    return *this;
}

eos::ColorHSV& eos::ColorHSV::operator-=(const eos::ColorHSV& color) {
    hue_ = std::fmod(hue_ - color.hue_, 360.0f);
    saturation_ -= color.saturation_;
    value_ -= color.value_;
    alpha_ -= color.alpha_;
    return *this;
}

eos::ColorHSV& eos::ColorHSV::operator*=(float scalar) {
    hue_ = std::fmod(hue_ * scalar, 360.0f);
    saturation_ *= scalar;
    if(saturation_ > 1.0f) saturation_ = 1.0f;
    value_ *= scalar;
    if(value_ > 1.0f) value_ = 1.0f;
    alpha_ *= scalar;
    if(alpha_ > 1.0f) alpha_ = 1.0f;
    return *this;
}

eos::ColorHSV& eos::ColorHSV::operator/=(float scalar) {
    hue_ = std::fmod(hue_ * scalar, 360.0f);
    saturation_ /= scalar;
    if(saturation_ > 1.0f) saturation_ = 1.0f;
    value_ /= scalar;
    if(value_ > 1.0f) value_ = 1.0f;
    alpha_ /= scalar;
    if(alpha_ > 1.0f) alpha_ = 1.0f;
    return *this;
}
