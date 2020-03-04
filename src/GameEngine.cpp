//
// Created by jakob on 10.09.19.
//

#include "../include/eos/GameEngine.hpp"
#include <thread>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/format.hpp>

static void error_callback(int error, const char* description){
    BOOST_LOG_TRIVIAL(error) << description;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    static_cast<eos::GameEngine*>(glfwGetWindowUserPointer(window))->stateManager.currentState()->input(key, scancode, action, mods);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    static_cast<eos::GameEngine*>(glfwGetWindowUserPointer(window))->stateManager.currentState()->resize(width, height);
}

eos::GameEngine::GameEngine(const std::string& config_path) : config(config_path) {
    if(config.log.toFile)
        boost::log::add_file_log(
            boost::log::keywords::file_name = "%Y-%m-%d_%H-%M-%S.%N.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0)
        );

    if(!glfwInit()) BOOST_LOG_TRIVIAL(fatal) << "GLFW initialization failed";
    window = glfwCreateWindow(config.window.width, config.window.height, config.window.title.c_str(), nullptr /*glfwGetPrimaryMonitor()*/, nullptr);
    if(!window) {
        BOOST_LOG_TRIVIAL(fatal) << "Creating window failed";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    BOOST_LOG_TRIVIAL(trace) << "GLContext set";

    GLenum err = glewInit();
    if(GLEW_OK != err) BOOST_LOG_TRIVIAL(error) << boost::format("glewInit failed, error: %s") % glewGetErrorString(err);
    BOOST_LOG_TRIVIAL(info) << boost::format("GLEW Version: %s") % glewGetString(GLEW_VERSION);
    BOOST_LOG_TRIVIAL(info) << boost::format("OpenGL Version: %s") % glGetString(GL_VERSION);
    BOOST_LOG_TRIVIAL(debug) << boost::format("OpenGL Vendor: %s, Renderer: %s, Shanding Language Version: %s") % glGetString(GL_VENDOR) % glGetString(GL_RENDERER) % glGetString(GL_SHADING_LANGUAGE_VERSION);

    glViewport(0, 0, config.window.width, config.window.height);
    BOOST_LOG_TRIVIAL(trace) << boost::format("GLViewport: %ix%i") % config.window.width % config.window.height;

    glfwSwapInterval(0);
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    init();
}

void eos::GameEngine::target_fps(int fps, bool cap) {
    config.engine.targetFps = fps;
    fpu = config.engine.targetFps * dt;
    config.engine.capFps = cap;
}

void eos::GameEngine::target_ups(int ups) {
    config.engine.targetUps = ups;
    maxFrameTime = config.engine.targetUps * 25;
    dt = 1.0f / config.engine.targetUps;
    fpu = config.engine.targetFps * dt;
}

void eos::GameEngine::init() {
    dt = 1.0f / config.engine.targetUps;
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

    while(!glfwWindowShouldClose(window)) {
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
            frames++;
            fps++;
        }
    }

    stateManager.currentState()->cleanup();
    glfwTerminate();
    return true;
}
