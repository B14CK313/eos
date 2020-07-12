//
// Created by jakob on 10.09.19.
//

#include "../include/eos/GameEngine.hpp"
#include <thread>
#include <utility>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

static void error_callback(int error, const char* description){
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
    // Dirty workaround
    static_cast<eos::GameEngine*>(glfwGetWindowUserPointer(window))->stateManager.input(key, scancode, action, mods);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

eos::GameEngine::GameEngine(const std::string& configPath) : config(configPath) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks[0]->set_level(spdlog::level::trace);

    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(config.log.fileName, true));
    sinks[1]->set_level(spdlog::level::warn);

    auto defaultLogger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
    defaultLogger->set_level(spdlog::level::trace);
    defaultLogger->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] %v");
    spdlog::register_logger(defaultLogger);
    spdlog::set_default_logger(defaultLogger);
    SPDLOG_TRACE("Logger initialized");

    if(!glfwInit()) SPDLOG_CRITICAL("GLFW initialization failed");

    //Only use modern OpenGL (All legacy functions will return an error)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(config.window.width, config.window.height, config.window.title.c_str(), nullptr /*glfwGetPrimaryMonitor()*/, nullptr);
    if(!window) {
        SPDLOG_CRITICAL("Creating window failed");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    SPDLOG_TRACE("GLContext set");

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) SPDLOG_ERROR("gladLoadGL failed");
    //SPDLOG_INFO("glad Version: {}", );
    SPDLOG_INFO("OpenGL Version: {}", glGetString(GL_VERSION));
    SPDLOG_DEBUG("OpenGL Vendor: {}, Renderer: {}, Shading Language Version: {}", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION));

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    SPDLOG_TRACE("GLViewport: {}x{} (Config: {}x{})", width, height, config.window.width, config.window.height);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(0);
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

[[maybe_unused]] void eos::GameEngine::target_fps(int fps, bool cap) {
    config.engine.targetFps = fps;
    fpu_ = config.engine.targetFps * dt_;
    config.engine.capFps = cap;
}

[[maybe_unused]] void eos::GameEngine::target_ups(int ups) {
    config.engine.targetUps = ups;
    maxFrameTime_ = config.engine.targetUps * 25;
    dt_ = 1.0 / config.engine.targetUps;
    fpu_ = config.engine.targetFps * dt_;
}

void eos::GameEngine::get_window_size(int& width, int& height) const {
    glfwGetWindowSize(window, &width, &height);
}

void eos::GameEngine::get_window_size(glm::ivec2& dims) const {
    glfwGetWindowSize(window, &dims[0], &dims[1]);
}

glm::ivec2 eos::GameEngine::get_window_size() const {
    glm::ivec2 dim;
    glfwGetWindowSize(window, &dim[0], &dim[1]);
    return dim;
}

void eos::GameEngine::init(std::shared_ptr<IGameState> initialState) {
    stateManager.push_state(std::move(initialState));

    dt_ = 1.0 / config.engine.targetUps;
    fpu_ = config.engine.targetFps * dt_;
    maxFrameTime_ = config.engine.targetUps * 25;
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

    while(glfwWindowShouldClose(window) == GLFW_FALSE) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - previousTime;
        if(frameTime > maxFrameTime_) frameTime = maxFrameTime_; // Avoid Spiral of Death
        previousTime = currentTime;
        accumulator += frameTime;

        if(currentTime - prevSec >= 1.0f) {
#ifdef DEBUG
            std::printf("\rcurrentTime: %f, t: %f, _dt: %f, accumulator %f, frameTime: %f, FPS: %i, UPS: %i ", currentTime, t, dt_, accumulator, frameTime, fps, ups);
#endif //DEBUG
            fflush(stdout);
            fps = 0;
            ups = 0;
            prevSec = currentTime;
        }

        // Run update every dt
        while(accumulator >= dt_){
            glfwPollEvents();
            stateManager.update(t, dt_);
            accumulator -= dt_;
            t += dt_;

            updates++;
            ups++;
        }

        double interpolation = accumulator / dt_;

        // FPS Cap
        if(config.engine.capFps && updates * fpu_ <= frames) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            stateManager.render(interpolation);
            glfwSwapBuffers(window);
            frames++;
            fps++;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}
