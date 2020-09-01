//
// Created by jakob on 10.09.19.
//

#include "eos/core/GameEngine.hpp"
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <eos/core/ServiceProvider.h>

static void error_callback(int error, const char* description) {
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    eos::ServiceProvider::getStateManager().getState()->key_input(key, scancode, action, mods);
}

static void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    eos::ServiceProvider::getStateManager().getState()->mouse_input(xPos, yPos);
}

static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    eos::ServiceProvider::getStateManager().getState()->scroll_input(xOffset, yOffset);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

eos::GameEngine::GameEngine() {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks[0]->set_level(spdlog::level::trace);

    sinks.push_back(
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(eos::ServiceProvider::getConfig().log.fileName, true));
    sinks[1]->set_level(spdlog::level::warn);

    auto defaultLogger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
    defaultLogger->set_level(spdlog::level::trace);
    defaultLogger->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] %v");
    spdlog::register_logger(defaultLogger);
    spdlog::set_default_logger(defaultLogger);
    SPDLOG_TRACE("Logger initialized");

    //Only use modern OpenGL (All legacy functions will return an error)
    eos::ServiceProvider::provide(std::make_unique<eos::Window>(
            std::initializer_list<eos::Window::Hint>{{GLFW_CONTEXT_VERSION_MAJOR, 3},
                                                     {GLFW_CONTEXT_VERSION_MINOR, 3},
                                                     {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
                                                     {GLFW_SAMPLES,               4}}));

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) SPDLOG_ERROR("gladLoadGL failed");
    //SPDLOG_INFO("glad Version: {}", );
    SPDLOG_INFO("OpenGL Version: {}", glGetString(GL_VERSION));
    SPDLOG_DEBUG("OpenGL Vendor: {}, Renderer: {}, Shading Language Version: {}", glGetString(GL_VENDOR),
                 glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glm::ivec2 dims;
    eos::ServiceProvider::getWindow().get_size(dims);
    glViewport(0, 0, dims[0], dims[1]);
    SPDLOG_TRACE("GLViewport: {}x{} (Config: {}x{})", dims[0], dims[1], eos::ServiceProvider::getConfig().window.width,
                 eos::ServiceProvider::getConfig().window.height);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    eos::ServiceProvider::getWindow()
        .set_swap_interval(0)
        .set_error_callback(error_callback)
        .set_key_callback(key_callback)
        .set_cursor_pos_callback(mouse_callback)
        .set_scroll_callback(scroll_callback)
        .set_framebuffer_size_callback(framebuffer_size_callback);

    dt_ = 1.0 / eos::ServiceProvider::getConfig().engine.targetUps;
    fpu_ = eos::ServiceProvider::getConfig().engine.targetFps * dt_;
    maxFrameTime_ = eos::ServiceProvider::getConfig().engine.targetUps * 25;
}

[[maybe_unused]] void eos::GameEngine::target_fps(int fps, bool cap) {
    eos::ServiceProvider::getConfig().engine.targetFps = fps;
    fpu_ = eos::ServiceProvider::getConfig().engine.targetFps * dt_;
    eos::ServiceProvider::getConfig().engine.capFps = cap;
}

[[maybe_unused]] void eos::GameEngine::target_ups(int ups) {
    eos::ServiceProvider::getConfig().engine.targetUps = ups;
    maxFrameTime_ = eos::ServiceProvider::getConfig().engine.targetUps * 25;
    dt_ = 1.0 / eos::ServiceProvider::getConfig().engine.targetUps;
    fpu_ = eos::ServiceProvider::getConfig().engine.targetFps * dt_;
}

bool eos::GameEngine::run() {
    double previousTime = glfwGetTime();
    double accumulator = 0.0f;
    double t = 0.0f;
    int frames = 0;
    int updates = 0;

    double prevSec = 0;
    int fps = 0;
    int ups = 0;

    SPDLOG_TRACE("Starting game loop...");

    while (!eos::ServiceProvider::getWindow().get_should_close()) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - previousTime;
        if (frameTime > maxFrameTime_) frameTime = maxFrameTime_; // Avoid Spiral of Death
        previousTime = currentTime;
        accumulator += frameTime;

        if (currentTime - prevSec >= 1.0f) {
#ifdef DEBUG
            //std::printf("\rcurrentTime: %f, t: %f, _dt: %f, accumulator %f, frameTime: %f, FPS: %i, UPS: %i ", currentTime, t, dt_, accumulator, frameTime, fps, ups);
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
            glfwPollEvents();
            eos::ServiceProvider::getStateManager().getState()->update(t, dt_);
            accumulator -= dt_;
            t += dt_;

            updates++;
            ups++;
        }

        double interpolation = accumulator / dt_;

        // FPS Cap
        if (eos::ServiceProvider::getConfig().engine.capFps && updates * fpu_ <= frames) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            eos::ServiceProvider::getStateManager().getState()->render(interpolation);
            eos::ServiceProvider::getWindow().swap_buffers();
            frames++;
            fps++;
        }
    }

    ServiceProvider::cleanup();

    return true;
}

int eos::GameEngine::get_fps() const {
    return fps_;
}

int eos::GameEngine::get_ups() const {
    return ups_;
}
