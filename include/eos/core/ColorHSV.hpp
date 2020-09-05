//
// Created by jakob on 04.09.20.
//

#ifndef EOS_COLORHSV_HPP
#define EOS_COLORHSV_HPP

namespace eos {
    class ColorRGB;

    class ColorHSV {
    public:
        float hue_ = 0;
        float saturation_ = 0;
        float value_ = 0;
        float alpha_ = 0;

    public:
        ColorHSV() = default;

        ColorHSV(float hue, float saturation, float value, float alpha = 1.0f);

        ColorRGB to_rgb() const;

        operator ColorRGB() const;

        ColorHSV operator+(const ColorHSV& color) const;

        ColorHSV operator-(const ColorHSV& color) const;

        ColorHSV operator*(float scalar) const;

        ColorHSV operator/(float scalar) const;

        ColorHSV& operator+=(const ColorHSV& color);

        ColorHSV& operator-=(const ColorHSV& color);

        ColorHSV& operator*=(float scalar);

        ColorHSV& operator/=(float scalar);
    };
}

#endif //EOS_COLORHSV_HPP
