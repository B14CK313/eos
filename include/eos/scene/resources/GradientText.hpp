//
// Created by jakob on 05.09.20.
//

#pragma once

#include <eos/scene/resources/Text.hpp>

namespace eos {
	class GradientText : private Text {
	public:
		explicit GradientText(std::experimental::observer_ptr<eos::Graphics> graphics, std::shared_ptr<Font> font);

		void render(const std::string& text, glm::vec2 pos, eos::ColorHSV gradientStartColor,
		            eos::ColorHSV gradientStopColor) const;
	};
}
