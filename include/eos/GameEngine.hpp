//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "StateManager.hpp"
#include "Config.hpp"

namespace eos {

    class GameEngine {
    public:
        explicit GameEngine(const std::string& configPath);

        void init(std::shared_ptr<IGameState> initialState);
        bool run();

        [[maybe_unused]] void target_fps(int fps, bool cap = true);

        [[maybe_unused]] void target_ups(int ups);

        StateManager stateManager{};
        Config config;
        GLFWwindow* window;

    private:
        double _maxFrameTime{};
        double _dt{};
        double _fpu{};
    };

}

#endif //EOS_GAMEENGINE_HPP
