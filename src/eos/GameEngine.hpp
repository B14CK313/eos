//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "StateManager.hpp"

namespace eos {

    class GameEngine {
    public:
        GameEngine(short width, short height, int targetUPS, int targetFPS, bool capFPS = true);

        bool run();

        void target_fps(int fps, bool cap = true);
        void target_ups(int ups);

        StateManager stateManager{};
        GLFWwindow* window;

    private:
        bool capFPS{};

        int targetUPS{};
        int targetFPS{};

        double maxFrameTime{};
        double dt{};
        double fpu{};
    };

}

#endif //EOS_GAMEENGINE_HPP
