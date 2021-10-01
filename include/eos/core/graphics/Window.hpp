//
// Created by jakob on 14.07.20.
//

#ifndef EOS_WINDOW_HPP
#define EOS_WINDOW_HPP


#include <string>
#include <glm/glm.hpp>
#include <SDL.h>
#include "eos/core/graphics/Graphics.hpp"
#include "eos/core/io/Config.hpp"

namespace eos {

	class Window {
	public:
		Window(std::experimental::observer_ptr<Config> config);

		virtual ~Window();

		void resize(int width, int height);
		[[nodiscard]] glm::ivec2 size() const;

		void maximize();
		[[nodiscard]] bool is_maximized() const;

		void minimize();
		[[nodiscard]] bool is_minimized() const;

		void restore();

		void set_resizeable(bool resizeable);
		[[nodiscard]] bool is_resizeable() const;

		void set_borderless(bool borderless);
		[[nodiscard]] bool is_borderless() const;

		void set_fullscreen(bool fullscreen);
		[[nodiscard]] bool is_fullscreen() const;

		void set_title(const std::string& title);
		[[nodiscard]] std::string title() const;

		[[nodiscard]] std::experimental::observer_ptr<Graphics> graphics() const;

	private:
		std::unique_ptr<Graphics> graphics_;
		SDL_Window* window_;
	};
}

#endif //EOS_WINDOW_HPP
