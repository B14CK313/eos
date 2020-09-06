//
// Created by jakob on 14.07.20.
//

#include <spdlog/spdlog.h>
#include <eos/core/ServiceProvider.h>
#include "eos/scene/Window.h"

eos::Window::Window(const std::string& title, int width, int height) : size_(width, height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SPDLOG_CRITICAL("Failed to initialize SDL: {}", SDL_GetError());
    }

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

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window_) {
        SPDLOG_CRITICAL("Failed to create SDL_Window: {}", SDL_GetError());
        SDL_Quit();
        throw;
    }

    if(SDL_GLContext glContext = SDL_GL_CreateContext(window_); glContext != nullptr){
        SDL_GL_MakeCurrent(window_, glContext);
    } else {
        SPDLOG_CRITICAL("Failed to create OpenGL context: {}", SDL_GetError());
        SDL_Quit();
        throw;
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        SPDLOG_CRITICAL("Failed to initialize GLAD");
        SDL_Quit();
        throw;
    }

    SPDLOG_INFO("OpenGL\tVersion: {}.{}\n\t Shading Language Version: {}\n\t Vendor: {}\n\t Renderer: {}", GLVersion.major, GLVersion.minor, glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));

    update_size();
}

eos::Window::~Window() {
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void eos::Window::resize(SDL_WindowEvent event) {
    size_.x = event.data1;
    size_.y = event.data2;

    SDL_GL_GetDrawableSize(window_, &size_[0], &size_[1]);
    glViewport(0, 0, size_[0], size_[1]);
    SPDLOG_TRACE("GLViewport: {}x{} (Config: {}x{})", size_[0], size_[1], eos::ServiceProvider::getConfig().window.width,
                 eos::ServiceProvider::getConfig().window.height);
}

void eos::Window::swap() {
    SDL_GL_SwapWindow(window_);
}

void eos::Window::update_size() {
    SDL_GL_GetDrawableSize(window_, &size_[0], &size_[1]);
    glViewport(0, 0, size_[0], size_[1]);
    SPDLOG_TRACE("GLViewport: {}x{} (Config: {}x{})", size_[0], size_[1], eos::ServiceProvider::getConfig().window.width,
                 eos::ServiceProvider::getConfig().window.height);
}
