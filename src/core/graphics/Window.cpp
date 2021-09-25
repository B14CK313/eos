//
// Created by jakob on 14.07.20.
//

#include <spdlog/spdlog.h>
#include "eos/core/ServiceProvider.hpp"
#include "eos/core/graphics/Window.hpp"
#include "eos/core/graphics/GraphicsOpenGL.hpp"
#include "eos/core/graphics/GraphicsVulkan.hpp"

eos::Window::Window(const std::string& title, int width, int height, Graphics::Type type) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SPDLOG_CRITICAL("Failed to initialize SDL: {}", SDL_GetError());
	}

	switch (type) {
		case Graphics::Type::OPENGL:
			graphics_ = std::make_unique<GraphicsOpenGL>();
			break;
		case Graphics::Type::VULKAN:
			graphics_ = std::make_unique<GraphicsVulkan>();
			break;
	}

	graphics_->setup(title);

	window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
	                           static_cast<int>(type));
	if (!window_) {
		SPDLOG_CRITICAL("Failed to create SDL_Window: {}", SDL_GetError());
		SDL_Quit();
		throw;
	}

	graphics_->instantiate(std::experimental::make_observer<>(window_));
}

eos::Window::~Window() {
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void eos::Window::resize(int width, int height) {
	SDL_SetWindowSize(window_, width, height);
	graphics_->resize(width, height);
}

glm::ivec2 eos::Window::size() const {
	glm::ivec2 size{};
	SDL_GetWindowSize(window_, &size[0], &size[1]);
	return size;
}

void eos::Window::maximize() {
	SDL_MaximizeWindow(window_);
}

bool eos::Window::is_maximized() const {
	return SDL_GetWindowFlags(window_) & SDL_WINDOW_MAXIMIZED;
}

void eos::Window::minimize() {
	SDL_MinimizeWindow(window_);
}

bool eos::Window::is_minimized() const {
	return SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED;
}

void eos::Window::restore() {
	SDL_RestoreWindow(window_);
}


void eos::Window::set_resizeable(bool resizeable) {
	SDL_SetWindowResizable(window_, static_cast<SDL_bool>(resizeable));
}

bool eos::Window::is_resizeable() const {
	return SDL_GetWindowFlags(window_) & SDL_WINDOW_RESIZABLE;
}

void eos::Window::set_borderless(bool borderless) {
	SDL_SetWindowBordered(window_, static_cast<SDL_bool>(borderless));
}

bool eos::Window::is_borderless() const {
	return SDL_GetWindowFlags(window_) & SDL_WINDOW_BORDERLESS;
}

void eos::Window::set_fullscreen(bool fullscreen) {
	SDL_SetWindowFullscreen(window_, static_cast<SDL_bool>(fullscreen));
}

bool eos::Window::is_fullscreen() const {
	return SDL_GetWindowFlags(window_) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void eos::Window::set_title(const std::string& title) {
	SDL_SetWindowTitle(window_, title.c_str());
}

std::string eos::Window::title() const {
	return SDL_GetWindowTitle(window_);
}

std::experimental::observer_ptr<eos::Graphics> eos::Window::graphics() const {
	return std::experimental::make_observer(graphics_.get());
}
