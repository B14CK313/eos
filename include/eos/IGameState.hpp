//
// Created by jakob on 23.09.19.
//

#ifndef EOS_IGAMESTATE_HPP
#define EOS_IGAMESTATE_HPP

#include <memory>

namespace eos {

    class GameEngine;

    class IGameState {
    public:
        virtual ~IGameState() = default;

        virtual bool init() = 0;

        virtual bool cleanup() = 0;

        virtual void onEnter() = 0;

        virtual void onExit() = 0;

        virtual void resize(int width, int height) = 0;

        virtual void input(int key, int scancode, int action, int mods) = 0;

        virtual void update(double t, double dt) = 0;

        virtual void render(double interpolation) = 0;

    public:
        std::shared_ptr<GameEngine> gameEngine;
    };

}

#endif //EOS_IGAMESTATE_HPP
