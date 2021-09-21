//
// Created by jakob on 14.07.20.
//

#include <spdlog/spdlog.h>
#include "eos/core/ServiceProvider.h"
#include "eos/core/graphics/Window.hpp"

eos::Window::Window(const std::string& title, int width, int height, Graphics::Type type) : size_(width, height) {
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

	graphics_->setup();

	window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, static_cast<int>(type) | SDL_WINDOW_RESIZABLE);
	if(!window_) {
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
	size_.x = width;
	size_.y = height;

	SPDLOG_TRACE("Resized window to {}x{} (Config: {}x{})", width, height,
				 eos::ServiceProvider::getConfig().window.width, eos::ServiceProvider::getConfig().window.height);

	graphics_->resize(width, height);
}

glm::ivec2 eos::Window::size() const {
	return size_;
}

std::experimental::observer_ptr<eos::Graphics> eos::Window::graphics() const {
	return std::experimental::make_observer(graphics_.get());
}

