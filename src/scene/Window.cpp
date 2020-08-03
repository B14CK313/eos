//
// Created by jakob on 14.07.20.
//

#include <spdlog/spdlog.h>
#include <eos/core/ServiceProvider.h>
#include "eos/scene/Window.h"

eos::Window::Window(int width, int height, const std::string& title, std::initializer_list<Hint> hints,
                    GLFWmonitor* monitor, GLFWwindow* share) {
    if (!glfwInit()) SPDLOG_CRITICAL("GLFW initialization failed");

    for (auto hint : hints) {
        glfwWindowHint(hint.name, hint.value);
    }

    glfwWindow_ = glfwCreateWindow(width, height, title.c_str(), monitor, share);

    if(!glfwWindow_) {
        SPDLOG_CRITICAL("Creating window failed");
        glfwTerminate();
    }

    set_context();
}

eos::Window::Window(std::initializer_list<Hint> hints) : Window(eos::ServiceProvider::getConfig().window.width, eos::ServiceProvider::getConfig().window.height, eos::ServiceProvider::getConfig().window.title, hints) {}

eos::Window::~Window() {
    glfwDestroyWindow(glfwWindow_);
    glfwTerminate();
}

const eos::Window& eos::Window::swap_buffers() const {
    glfwSwapBuffers(glfwWindow_);
    return *this;
}

const eos::Window& eos::Window::set_context() const {
    glfwMakeContextCurrent(glfwWindow_);
    SPDLOG_TRACE("GLContext set");
    return *this;
}

const eos::Window& eos::Window::set_swap_interval(int interval) const {
    glfwSwapInterval(interval);
    return *this;
}

const eos::Window& eos::Window::set_input_mode(int mode, int value) {
    glfwSetInputMode(glfwWindow_, mode, value);
    return *this;
}

const eos::Window& eos::Window::set_error_callback(GLFWerrorfun callback) const {
    glfwSetErrorCallback(callback);
    return *this;
}

const eos::Window& eos::Window::set_key_callback(GLFWkeyfun callback) const {
    glfwSetKeyCallback(glfwWindow_, callback);
    return *this;
}

const eos::Window& eos::Window::set_cursor_pos_callback(GLFWcursorposfun callback) const {
    glfwSetCursorPosCallback(glfwWindow_, callback);
    return *this;
}

const eos::Window& eos::Window::set_scroll_callback(GLFWscrollfun callback) const {
    glfwSetScrollCallback(glfwWindow_, callback);
    return *this;
}

const eos::Window& eos::Window::set_framebuffer_size_callback(GLFWframebuffersizefun callback) const {
    glfwSetFramebufferSizeCallback(glfwWindow_, callback);
    return *this;
}

const eos::Window& eos::Window::set_should_close() const {
    glfwSetWindowShouldClose(glfwWindow_, GLFW_TRUE);
    return *this;
}

bool eos::Window::get_should_close() const {
    return glfwWindowShouldClose(glfwWindow_) == GLFW_TRUE;
}

const eos::Window& eos::Window::get_size(int& width, int& height) const {
    glfwGetWindowSize(glfwWindow_, &width, &height);
    return *this;
}

const eos::Window& eos::Window::get_size(glm::ivec2& dims) const {
    glfwGetWindowSize(glfwWindow_, &dims[0], &dims[1]);
    return *this;
}

glm::ivec2 eos::Window::get_size() const {
    glm::ivec2 dim;
    glfwGetWindowSize(glfwWindow_, &dim[0], &dim[1]);
    return dim;
}
