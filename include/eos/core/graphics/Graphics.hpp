//
// Created by jakob on 20.09.21.
//

#pragma once

#include <SDL_video.h>
#include <experimental/memory>

namespace eos {
	class Window;

	class Graphics {
	public:
		enum class Type {
			OPENGL = SDL_WINDOW_OPENGL,
			VULKAN = SDL_WINDOW_VULKAN
		};

		virtual ~Graphics() = default;

		virtual void setup() = 0;

		virtual void instantiate(std::experimental::observer_ptr<SDL_Window> window) = 0;

		virtual void swap() = 0;

		virtual void vsync(bool value) = 0;

		virtual void resize(int width, int height) = 0;

		[[nodiscard]] virtual glm::ivec2 size() const = 0;

	protected:
		std::experimental::observer_ptr<SDL_Window> window_;
	};

	class GraphicsOpenGL : public Graphics {
	public:
		void setup() override;

		void instantiate(std::experimental::observer_ptr<SDL_Window> window) override;

		void swap() override;

		void vsync(bool value) override;

		void resize(int width, int height) override;

		[[nodiscard]] glm::ivec2 size() const override;
	};

	class GraphicsVulkan : public Graphics {
	public:
		void setup() override;

		void instantiate(std::experimental::observer_ptr<SDL_Window> window) override;

		void swap() override;

		void vsync(bool value) override;

		void resize(int width, int height) override;

		[[nodiscard]] glm::ivec2 size() const override;
	};

}
