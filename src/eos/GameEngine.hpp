//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <glbinding/gl/gl.h>
#include <GLFW/glfw3.h>
#include "StateManager.hpp"

namespace eos {

    class GameEngine {
    public:
        GameEngine(short width, short height);

        bool run();

        StateManager stateManager;
        GLFWwindow* window;
    };

}

#endif //EOS_GAMEENGINE_HPP
