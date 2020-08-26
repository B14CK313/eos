//
// Created by jakob on 23.09.19.
//

#ifndef EOS_IGAMESTATE_HPP
#define EOS_IGAMESTATE_HPP

#include <memory>
#include <GLFW/glfw3.h>

namespace eos {

    class GameEngine;

    class IGameState {
    public:
        virtual ~IGameState() = default;

        virtual void on_enter() = 0;

        virtual void on_exit() = 0;

        virtual void resize(int width, int height) = 0;

        virtual void key_input(int key, int scancode, int action, int mods) = 0;

        virtual void mouse_input(double x, double y) = 0;

        virtual void scroll_input(double xoffset, double yoffset) = 0;

        virtual void update(double t, double dt) = 0;

        virtual void render(double interpolation) = 0;
    };

}

#endif //EOS_IGAMESTATE_HPP