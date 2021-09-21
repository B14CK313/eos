//
// Created by jakob on 14.07.20.
//

#ifndef EOS_WINDOW_HPP
#define EOS_WINDOW_HPP


#include <string>
#include <glm/glm.hpp>
#include <SDL.h>
#include "eos/core/graphics/Graphics.hpp"

namespace eos {

	class Window {
	public:
		Window(const std::string& title, int width, int height, Graphics::Type type);

		virtual ~Window();

		void resize(int width, int height);

		[[nodiscard]] glm::ivec2 size() const;

		[[nodiscard]] std::experimental::observer_ptr<Graphics> graphics() const;

	private:
		glm::ivec2 size_;
		std::unique_ptr<Graphics> graphics_;
		SDL_Window* window_;
	};
}

#endif //EOS_WINDOW_HPP
