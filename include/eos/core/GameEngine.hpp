//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "eos/scene/StateManager.hpp"
#include "eos/core/io/Config.hpp"

namespace eos {

    class GameEngine {
    public:
        explicit GameEngine();

        bool run();

        void target_fps(int fps, bool cap = true);

        void target_ups(int ups);

    private:
        double maxFrameTime_{};
        double dt_{};
        double fpu_{};
    };

}

#endif //EOS_GAMEENGINE_HPP
