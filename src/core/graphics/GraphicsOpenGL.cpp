//
// Created by jakob on 20.09.21.
//

#include <spdlog/spdlog.h>
#include <SDL.h>
#include <glad/glad.h>
#include "eos/core/graphics/Window.hpp"

void eos::GraphicsOpenGL::setup(std::string_view title) {
	SDL_GL_LoadLibrary(nullptr);
#ifdef DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);
#else //DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif //DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // TODO: Config setting
}

void eos::GraphicsOpenGL::instantiate(std::experimental::observer_ptr<SDL_Window> window) {
	window_ = window;

	if (SDL_GLContext glContext = SDL_GL_CreateContext(window_.get()); glContext != nullptr) {
		SDL_GL_MakeCurrent(window_.get(), glContext);
	} else {
		SPDLOG_CRITICAL("Failed to create OpenGL context: {}", SDL_GetError());
		SDL_Quit();
		throw;
	}

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		SPDLOG_CRITICAL("Failed to initialize GLAD");
		SDL_Quit();
		throw;
	}

	SPDLOG_INFO("OpenGL\tVersion: {}.{}\n\t Shading Language Version: {}\n\t Vendor: {}\n\t Renderer: {}",
	            GLVersion.major, GLVersion.minor, glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_VENDOR),
	            glGetString(GL_RENDERER));
}

void eos::GraphicsOpenGL::swap() {
	SDL_GL_SwapWindow(window_.get());
}

void eos::GraphicsOpenGL::vsync(bool value) {
	SDL_GL_SetSwapInterval(value ? 1 : 0);
}

void eos::GraphicsOpenGL::resize(int width, int height) {
	glViewport(0, 0, width, height);
	auto viewport_size = size();
	SPDLOG_TRACE("Resized GLViewport to: {}x{}", viewport_size.x, viewport_size.y);
}

glm::ivec2 eos::GraphicsOpenGL::size() const {
	glm::ivec2 size{};
	SDL_GL_GetDrawableSize(window_.get(), &size[0], &size[1]);
	return size;
}

