//
// Created by jakob on 10.09.19.
//

#include "eos/core/GameEngine.hpp"
#include <sstream>
#include <thread>
#include <spdlog/spdlog.h>
#include <eos/core/ServiceProvider.hpp>

void APIENTRY
glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
              const void* userParam) {
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::stringstream sstream;
	sstream << "Debug message (" << id << "): " << message << std::endl;

	switch (source) {
		case GL_DEBUG_SOURCE_API:
			sstream << "Source: API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			sstream << "Source: Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			sstream << "Source: Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			sstream << "Source: Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			sstream << "Source: Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			sstream << "Source: Other";
			break;
	}
	sstream << std::endl;

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			sstream << "Type: Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			sstream << "Type: Deprecated Behaviour";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			sstream << "Type: Undefined Behaviour";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			sstream << "Type: Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			sstream << "Type: Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			sstream << "Type: Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			sstream << "Type: Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			sstream << "Type: Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER:
			sstream << "Type: Other";
			break;
	}
	sstream << std::endl;

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			sstream << "Severity: high";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			sstream << "Severity: medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			sstream << "Severity: low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			sstream << "Severity: notification";
			break;
	}
	sstream << std::endl;

	SPDLOG_ERROR(sstream.str());
}

// TODO: use namespaced intX_t (e.g. std::int8_t instead of int8_t) and include <cstdint>

eos::GameEngine::GameEngine() {
	config_ = eos::ServiceProvider::getConfigPtr();
	window_ = eos::ServiceProvider::getWindowPtr();
	stateManager_ = eos::ServiceProvider::getStateManagerPtr();

	window_->graphics()->vsync(config_->engine.vsync);

	/*
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
#ifdef DEBUG
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if ((GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3)) && flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        SPDLOG_TRACE("GL Debug Output enabled");
    }
#endif //DEBUG
*/

	dt_ = config_->engine.targetUps / 10.0f;
	fpu_ = config_->engine.targetFps / static_cast<float>(config_->engine.targetUps);
	maxFrameTime_ = dt_ * 25;
}

[[maybe_unused]] void eos::GameEngine::target_fps(int fps, bool cap) {
	config_->engine.targetFps = fps;
	fpu_ = config_->engine.targetFps / static_cast<float>(config_->engine.targetUps);
	config_->engine.capFps = cap;
}

[[maybe_unused]] void eos::GameEngine::target_ups(int ups) {
	config_->engine.targetUps = ups;
	dt_ = config_->engine.targetUps / 10.0f;
	fpu_ = config_->engine.targetFps / static_cast<float>(config_->engine.targetUps);
	maxFrameTime_ = dt_ * 25;
}

bool eos::GameEngine::run() {
	uint32_t previousTime = SDL_GetTicks();
	uint32_t accumulator = 0;
	double t = 0.0f;
	int frames = 0;
	int updates = 0;

	uint32_t prevSec = 0;
	int fps = 0;
	int ups = 0;

	auto graphics = window_->graphics();

	SPDLOG_TRACE("Starting game loop...");

	while (!quit_) {
		uint32_t currentTime = SDL_GetTicks();
		uint32_t frameTime = currentTime - previousTime;
		if (frameTime > maxFrameTime_) frameTime = maxFrameTime_; // Avoid Spiral of Death
		previousTime = currentTime;
		accumulator += frameTime;
		SDL_Event event;

		if (currentTime - prevSec >= 1000) {
#ifdef DEBUG
			//std::printf("\rcurrentTime: %u, t: %f, _dt: %f, accumulator %u, frameTime: %u, FPS: %i, UPS: %i ", currentTime, t, dt_, accumulator, frameTime, fps, ups);
#endif //DEBUG
			fps_ = fps;
			ups_ = ups;
			fflush(stdout);
			fps = 0;
			ups = 0;
			prevSec = currentTime;
		}

		// Run update every dt
		while (accumulator >= dt_) {
			if (SDL_PollEvent(&event)) {
				auto state = stateManager_->getState();
				do {
					switch (event.type) {
						// Application events
						case SDL_QUIT:
							state->quit();
							break;
						case SDL_APP_TERMINATING:
							quit_ = true;
							break;
						case SDL_APP_LOWMEMORY:
							break;
						case SDL_APP_WILLENTERBACKGROUND:
							break;
						case SDL_APP_DIDENTERBACKGROUND:
							break;
						case SDL_APP_WILLENTERFOREGROUND:
							break;
						case SDL_APP_DIDENTERFOREGROUND:
							break;
							// Window events
						case SDL_WINDOWEVENT: {
							switch (event.window.event) {
								case SDL_WINDOWEVENT_SHOWN:
									break;
								case SDL_WINDOWEVENT_HIDDEN:
									break;
								case SDL_WINDOWEVENT_EXPOSED:
									break;
								case SDL_WINDOWEVENT_MOVED:
									break;
								case SDL_WINDOWEVENT_RESIZED:
									window_->graphics()->resize(event.window.data1, event.window.data2);
									state->resize(event.window.data1, event.window.data2);
									break;
								case SDL_WINDOWEVENT_SIZE_CHANGED:
									break;
								case SDL_WINDOWEVENT_MINIMIZED:
									break;
								case SDL_WINDOWEVENT_MAXIMIZED:
									break;
								case SDL_WINDOWEVENT_RESTORED:
									break;
								case SDL_WINDOWEVENT_ENTER:
									break;
								case SDL_WINDOWEVENT_LEAVE:
									break;
								case SDL_WINDOWEVENT_FOCUS_GAINED:
									break;
								case SDL_WINDOWEVENT_FOCUS_LOST:
									break;
								case SDL_WINDOWEVENT_CLOSE:
									break;
								case SDL_WINDOWEVENT_TAKE_FOCUS:
									break;
								case SDL_WINDOWEVENT_HIT_TEST:
									break;
								default:
									break;
							}
							break;
						}
						case SDL_SYSWMEVENT:
							break;
							// Keyboard events
						case SDL_KEYDOWN:
							state->key_down(event.key);
							break;
						case SDL_KEYUP:
							state->key_up(event.key);
							break;
						case SDL_TEXTEDITING:
							break;
						case SDL_TEXTINPUT:
							break;
						case SDL_KEYMAPCHANGED:
							break;
							// Mouse events
						case SDL_MOUSEMOTION:
							state->mouse_motion(event.motion);
							break;
						case SDL_MOUSEBUTTONDOWN:
							state->mouse_button_down(event.button);
							break;
						case SDL_MOUSEBUTTONUP:
							state->mouse_button_up(event.button);
							break;
						case SDL_MOUSEWHEEL:
							state->mouse_wheel(event.wheel);
							break;
							// Joystick events
						case SDL_JOYAXISMOTION:
							break;
						case SDL_JOYBALLMOTION:
							break;
						case SDL_JOYHATMOTION:
							break;
						case SDL_JOYBUTTONDOWN:
							break;
						case SDL_JOYBUTTONUP:
							break;
						case SDL_JOYDEVICEADDED:
							break;
						case SDL_JOYDEVICEREMOVED:
							break;
							// Controller events
						case SDL_CONTROLLERAXISMOTION:
							break;
						case SDL_CONTROLLERBUTTONDOWN:
							break;
						case SDL_CONTROLLERBUTTONUP:
							break;
						case SDL_CONTROLLERDEVICEADDED:
							break;
						case SDL_CONTROLLERDEVICEREMOVED:
							break;
						case SDL_CONTROLLERDEVICEREMAPPED:
							break;
							// Touch events
						case SDL_FINGERDOWN:
							break;
						case SDL_FINGERUP:
							break;
						case SDL_FINGERMOTION:
							break;
							// Gesture events
						case SDL_DOLLARGESTURE:
							break;
						case SDL_DOLLARRECORD:
							break;
						case SDL_MULTIGESTURE:
							break;
							// Clipboard events
						case SDL_CLIPBOARDUPDATE:
							break;
							// Drag and drop events
						case SDL_DROPFILE:
							break;
						case SDL_DROPTEXT:
							break;
						case SDL_DROPBEGIN:
							break;
						case SDL_DROPCOMPLETE:
							break;
							// Audio hotplug events
						case SDL_AUDIODEVICEADDED:
							break;
						case SDL_AUDIODEVICEREMOVED:
							break;
							// Render events
						case SDL_RENDER_TARGETS_RESET:
							break;
						case SDL_RENDER_DEVICE_RESET:
							break;
							// User specified events
						case SDL_USEREVENT:
							break;
						case SDL_LASTEVENT:
							break;
						default:
							break;
					}
				} while (SDL_PollEvent(&event));
			}
			stateManager_->getState()->update(t, dt_);
			accumulator -= dt_;
			t += dt_;

			updates++;
			ups++;
		}

		double interpolation = accumulator / dt_;

		// FPS Cap
		if (config_->engine.capFps && updates * fpu_ <= frames) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		} else {
			stateManager_->getState()->render(interpolation);
			graphics->swap();
			frames++;
			fps++;
		}
	}

	ServiceProvider::cleanup();

	return true;
}

int eos::GameEngine::fps() const {
	return fps_;
}

int eos::GameEngine::ups() const {
	return ups_;
}
