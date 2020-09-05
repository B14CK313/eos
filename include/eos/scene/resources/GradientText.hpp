//
// Created by jakob on 05.09.20.
//

#ifndef EOS_GRADIENTTEXT_HPP
#define EOS_GRADIENTTEXT_HPP


#include <eos/scene/resources/Text.hpp>

namespace eos {
    class GradientText : private Text {
    public:
        explicit GradientText(std::shared_ptr<Font> font);

        void render(const std::string& text, glm::vec2 pos, eos::ColorHSV gradientStartColor, eos::ColorHSV gradientStopColor) const;
    };
}

#endif //EOS_GRADIENTTEXT_HPP
