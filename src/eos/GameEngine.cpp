//
// Created by jakob on 10.09.19.
//

#include <thread>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include "GameEngine.hpp"

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

eos::GameEngine::GameEngine(short width, short height) : stateManager() {
    if(!glfwInit()) BOOST_LOG_TRIVIAL(fatal) << "Could not initialize GLFW";
    window = glfwCreateWindow(width, height, "TEST", nullptr /*glfwGetPrimaryMonitor()*/, nullptr);
    if(!window) {
        BOOST_LOG_TRIVIAL(fatal) << "Could not create window";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    BOOST_LOG_TRIVIAL(trace) << "GLContext set";

    GLenum err = glewInit();
    if(GLEW_OK != err) BOOST_LOG_TRIVIAL(error) << boost::format("glewInit failed, error: %s") % glewGetErrorString(err);
    BOOST_LOG_TRIVIAL(info) << boost::format("GLEW Version: %s") % glewGetString(GLEW_VERSION);
    BOOST_LOG_TRIVIAL(info) << boost::format("OpenGL Version: %s") % glGetString(GL_VERSION);
    BOOST_LOG_TRIVIAL(debug) << boost::format("OpenGL Vendor: %s, Renderer: %s, Shanding Language Version: %s") % glGetString(GL_VENDOR) % glGetString(GL_RENDERER) % glGetString(GL_SHADING_LANGUAGE_VERSION);

    glViewport(0, 0, width, height);
    BOOST_LOG_TRIVIAL(trace) << boost::format("GLViewport: %ix%i") % width % height;

    glfwSwapInterval(0);
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

bool eos::GameEngine::run() {
    int frames = 0;
    int updates = 0;
    double prevSec = 0;

    const double dt = 0.01f;
    const double maxFrameTime = 0.25f;

    double t = 0.0f;
    double previousTime = glfwGetTime();
    double accumulator = 0.0f;

    bool drawn = false;

    while(!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - previousTime;
        if(frameTime > maxFrameTime) frameTime = maxFrameTime; // Avoid Spiral of Death
        previousTime = currentTime;
        accumulator += frameTime;

        //std::printf("\rcurrentTime: %f,\tt: %f,\tdt: %f,\taccumulator %f,\tframeTime: %f,\tFPS: %f", currentTime, t, dt, accumulator, frameTime, 1/frameTime);

        if(currentTime - prevSec >= 1.0f) {
            std::printf("\r                                                                                                                          ,\tFrames: %i,\tUpdates: %i", frames, updates);
            frames = 0;
            updates = 0;
            prevSec = currentTime;
        }

        // Run update every dt
        while(accumulator >= dt){
            std::printf("t: %f, dt: %f, accumulator %f, frameTime: %f, FPS: %f\r", t, dt, accumulator, frameTime, 1/frameTime);

            glfwPollEvents();
            stateManager.currentState()->update(t, dt);
            accumulator -= dt;
            t += dt;

            drawn = false;

            updates++;
        }

        double interpolation = accumulator / dt;

        // Fix render calls to update, rendering without update is pointless
        if(drawn) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            stateManager.currentState()->render(interpolation);
            drawn = true;

            frames++;
        }
    }

    stateManager.currentState()->cleanup();
    glfwTerminate();
    return true;
}
