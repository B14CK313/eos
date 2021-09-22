//
// Created by jakob on 09.09.20.
//

#pragma once

#include <eos/core/ColorRGB.hpp>

namespace eos {
    class Material {
    public:
        eos::ColorRGB ambient_;
        eos::ColorRGB diffuse_;
        eos::ColorRGB specular_;
        float shininess_;

    public:
        Material(const ColorRGB& ambient, const ColorRGB& diffuse, const ColorRGB& specular, float shininess);

        unsigned int use() const;
    private:
        unsigned int ubo_;

    private:
    };
}
