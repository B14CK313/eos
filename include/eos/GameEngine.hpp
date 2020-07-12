//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "StateManager.hpp"
#include "Config.hpp"

namespace eos {

    class GameEngine {
    public:
        explicit GameEngine(const std::string& configPath);

        void init(std::shared_ptr<IGameState> initialState);
        bool run();

        void target_fps(int fps, bool cap = true);

        void target_ups(int ups);

        void get_window_size(int& width, int& height) const;

        void get_window_size(glm::ivec2& dims) const;

        [[nodiscard]] glm::ivec2 get_window_size() const;

        StateManager stateManager{};
        Config config;
        GLFWwindow* window;

    private:
        double maxFrameTime_{};
        double dt_{};
        double fpu_{};
    };

}

#endif //EOS_GAMEENGINE_HPP
