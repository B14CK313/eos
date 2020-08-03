//
// Created by jakob on 14.07.20.
//

#ifndef EOS_WINDOW_H
#define EOS_WINDOW_H


#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace eos {

    class Window {
    public:
        struct Hint {
            int name;
            int value;
        };

        Window(int width, int height, const std::string& title,
               std::initializer_list<Hint> hints = {{GLFW_CONTEXT_VERSION_MAJOR, 3},
                                                    {GLFW_CONTEXT_VERSION_MINOR, 3},
                                                    {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE}},
               GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

        Window(std::initializer_list<Hint> hints);

        virtual ~Window();

        const eos::Window& swap_buffers() const;

        const eos::Window& set_context() const;

        const eos::Window& set_swap_interval(int interval) const;

        const eos::Window& set_input_mode(int mode, int value);

        const eos::Window& set_error_callback(GLFWerrorfun callback) const;

        const eos::Window& set_key_callback(GLFWkeyfun callback) const;

        const eos::Window& set_cursor_pos_callback(GLFWcursorposfun callback) const;

        const eos::Window& set_scroll_callback(GLFWscrollfun callback) const;

        const eos::Window& set_framebuffer_size_callback(GLFWframebuffersizefun callback) const;

        const eos::Window& set_should_close() const;

        [[nodiscard]] bool get_should_close() const;

        const eos::Window& get_size(int& width, int& height) const;

        const eos::Window& get_size(glm::ivec2& dims) const;

        [[nodiscard]] glm::ivec2 get_size() const;

    private:
        GLFWwindow* glfwWindow_;
    };
}

#endif //EOS_WINDOW_H
