//
// Created by jakob on 24.09.19.
//

#ifndef GLFW_GAMEENGINE_INGAMESTATE_HPP
#define GLFW_GAMEENGINE_INGAMESTATE_HPP


#include "../../eos/IGameState.hpp"

namespace game {

    class IngameState : public eos::IGameState {
    public:
        bool init(std::shared_ptr<eos::GameEngine> gameEngine) override;

        bool cleanup() override;

        void onEnter() override;

        void onExit() override;

        void resize(int width, int height) override;

        void input(int key, int scancode, int action, int mods) override;

        void update(double t, double dt) override;

        void render(double interpolation) override;
    };

}

#endif //GLFW_GAMEENGINE_INGAMESTATE_HPP
