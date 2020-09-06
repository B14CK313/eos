//
// Created by jakob on 10.09.19.
//

#ifndef EOS_GAMEENGINE_HPP
#define EOS_GAMEENGINE_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <eos/scene/Window.h>
#include "eos/scene/StateManager.hpp"
#include "eos/core/io/Config.hpp"

namespace eos {

    class GameEngine {
    public:
        bool quit_ = false;

    public:
        explicit GameEngine();

        bool run();

        void target_fps(int fps, bool cap = true);

        void target_ups(int ups);

        int get_fps() const;

        int get_ups() const;

    private:
        std::shared_ptr<eos::Config> config_;

        std::shared_ptr<eos::Window> window_;

        std::shared_ptr<eos::StateManager> stateManager_;


        double maxFrameTime_{};
        double dt_{};
        double fpu_{};

        int fps_;
        int ups_;
    };

}

#endif //EOS_GAMEENGINE_HPP
