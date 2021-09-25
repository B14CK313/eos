//
// Created by jakob on 24.09.21.
//

#pragma once

#include "eos/core/graphics/Graphics.hpp"

namespace eos {

	class GraphicsOpenGL : public Graphics {
	public:
		void setup(std::string_view title) override;

		void instantiate(std::experimental::observer_ptr<SDL_Window> window) override;

		void swap() override;

		void vsync(bool value) override;

		void resize(int width, int height) override;

		[[nodiscard]] glm::ivec2 size() const override;
	};

}