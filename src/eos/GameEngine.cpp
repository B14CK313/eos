//
// Created by jakob on 10.09.19.
//

#include <boost/log/trivial.hpp>
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
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

bool eos::GameEngine::run() {
    const double dt = 0.01f;
    const double maxFrameTime = 0.25f;

    double t = 0.0f;
    double previousTime = glfwGetTime();
    double accumulator = 0.0f;

    while(!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - previousTime;
        if(frameTime > maxFrameTime) frameTime = maxFrameTime; // Avoid Spiral of Death
        previousTime = currentTime;
        accumulator += frameTime;

        //std::printf("t: %f, dt: %f, accumulator %f, frameTime: %f, FPS: %f\r", t, dt, accumulator, frameTime, 1/frameTime);

        // Run update every dt
        while(accumulator >= dt){
            std::printf("t: %f, dt: %f, accumulator %f, frameTime: %f, FPS: %f\r", t, dt, accumulator, frameTime, 1/frameTime);

            glfwPollEvents();
            stateManager.currentState()->update(t, dt);
            accumulator -= dt;
            t += dt;
        }

        double interpolation = accumulator / dt;
        stateManager.currentState()->render(interpolation);
    }

    stateManager.currentState()->cleanup();
    glfwTerminate();
    return true;
}
