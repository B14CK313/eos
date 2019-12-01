//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "StateManager.hpp"
#include "Config.hpp"

namespace eos {

    class GameEngine {
    public:
        explicit GameEngine(const std::string& config_path, const std::string& default_config = eos::Config::DEFAULT_CONFIG);

        bool run();

        void target_fps(int fps, bool cap = true);
        void target_ups(int ups);

        StateManager stateManager{};
        Config config;
        GLFWwindow* window;

    private:
        bool capFPS;

        int targetUPS;
        int targetFPS;

        double maxFrameTime{};
        double dt{};
        double fpu{};
    };

}

#endif //EOS_GAMEENGINE_HPP
