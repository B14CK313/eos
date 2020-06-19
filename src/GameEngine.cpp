//
// Created by jakob on 10.09.19.
//

#include "../include/eos/GameEngine.hpp"
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

static void error_callback(int error, const char* description){
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

eos::GameEngine::GameEngine(const std::string& config_path) : config(config_path) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    //console_sink->set_level(spdlog::level::trace);

    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.log", true));
    //file_sink->set_level(spdlog::level::warn);

    auto default_logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
    default_logger->flush_on(spdlog::level::trace);
    spdlog::register_logger(default_logger);
    spdlog::set_default_logger(default_logger);
    SPDLOG_TRACE("Logger initialized");

    if(!glfwInit()) SPDLOG_CRITICAL("GLFW initialization failed");

    window = glfwCreateWindow(config.window.width, config.window.height, config.window.title.c_str(), nullptr /*glfwGetPrimaryMonitor()*/, nullptr);
    if(!window) {
        SPDLOG_CRITICAL("Creating window failed");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    SPDLOG_TRACE("GLContext set");

    GLenum err = glewInit();
    if(GLEW_OK != err) SPDLOG_ERROR("glewInit failed, error: {}", glewGetErrorString(err));
    SPDLOG_INFO("GLEW Version: {}", glewGetString(GLEW_VERSION));
    SPDLOG_INFO("OpenGL Version: {}", glGetString(GL_VERSION));
    SPDLOG_DEBUG("OpenGL Vendor: {}, Renderer: {}, Shanding Language Version: {}", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glViewport(0, 0, config.window.width, config.window.height);
    SPDLOG_TRACE("GLViewport: {}x{}", config.window.width, config.window.height);

    glfwSwapInterval(0);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

[[maybe_unused]] void eos::GameEngine::target_fps(int fps, bool cap) {
    config.engine.targetFps = fps;
    fpu = config.engine.targetFps * dt;
    config.engine.capFps = cap;
}

[[maybe_unused]] void eos::GameEngine::target_ups(int ups) {
    config.engine.targetUps = ups;
    maxFrameTime = config.engine.targetUps * 25;
    dt = 1.0 / config.engine.targetUps;
    fpu = config.engine.targetFps * dt;
}

void eos::GameEngine::init(std::shared_ptr<IGameState> initialState) {
    stateManager.pushState(initialState);

    dt = 1.0 / config.engine.targetUps;
    fpu = config.engine.targetFps * dt;
    maxFrameTime = config.engine.targetUps * 25;
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
        if(frameTime > maxFrameTime) frameTime = maxFrameTime; // Avoid Spiral of Death
        previousTime = currentTime;
        accumulator += frameTime;

        if(currentTime - prevSec >= 1.0f) {
            std::printf("\rcurrentTime: %f, t: %f, dt: %f, accumulator %f, frameTime: %f, FPS: %i, UPS: %i ", currentTime, t, dt, accumulator, frameTime, fps, ups);
            fflush(stdout);
            fps = 0;
            ups = 0;
            prevSec = currentTime;
        }

        // Run update every dt
        while(accumulator >= dt){
            //std::printf("t: %f, dt: %f, accumulator %f, frameTime: %f, FPS: %f\r", t, dt, accumulator, frameTime, 1/frameTime);

            glfwPollEvents();
            stateManager.currentState()->update(t, dt);
            accumulator -= dt;
            t += dt;

            updates++;
            ups++;
        }

        double interpolation = accumulator / dt;

        // FPS Cap
        if(config.engine.capFps && updates * fpu <= frames) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            stateManager.currentState()->render(interpolation);
            glfwSwapBuffers(window);
            frames++;
            fps++;
        }
    }

    stateManager.currentState()->cleanup();
    glfwTerminate();
    return true;
}
