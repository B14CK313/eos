//
// Created by jakob on 23.09.19.
//

#pragma once

#include <memory>
#include <SDL_events.h>

namespace eos {

    class GameEngine;

    class GameState {
    public:
        virtual ~GameState() = default;

        virtual void on_enter() = 0;

        virtual void on_exit() = 0;

        virtual void quit();

        virtual void key_down(SDL_KeyboardEvent event);

        virtual void key_up(SDL_KeyboardEvent event);

        virtual void mouse_motion(SDL_MouseMotionEvent event);

        virtual void mouse_button_down(SDL_MouseButtonEvent event);

        virtual void mouse_button_up(SDL_MouseButtonEvent event);

        virtual void mouse_wheel(SDL_MouseWheelEvent event);

        virtual void resize(int width, int height);

        virtual void update(double t, double dt) = 0;

        virtual void render(double interpolation) = 0;
    };

}
