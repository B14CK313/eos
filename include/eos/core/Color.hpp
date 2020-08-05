//
// Created by jakob on 03.08.20.
//

#ifndef EOS_COLOR_HPP
#define EOS_COLOR_HPP

#include <glm/glm.hpp>

namespace eos {

    class Color : public glm::vec4 {
        using glm::vec4::vec4;
    };
}

#endif //EOS_COLOR_HPP
