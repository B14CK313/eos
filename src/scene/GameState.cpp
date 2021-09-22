//
// Created by jakob on 06.09.20.
//

#include <eos/core/ServiceProvider.hpp>
#include "eos/scene/GameState.hpp"

void eos::GameState::quit() {
    on_exit();
    eos::ServiceProvider::getGameEngine().quit_ = true;
}

void eos::GameState::key_down(SDL_KeyboardEvent event) {}

void eos::GameState::key_up(SDL_KeyboardEvent event) {}

void eos::GameState::mouse_motion(SDL_MouseMotionEvent event) {}

void eos::GameState::mouse_button_down(SDL_MouseButtonEvent event) {}

void eos::GameState::mouse_button_up(SDL_MouseButtonEvent event) {}

void eos::GameState::mouse_wheel(SDL_MouseWheelEvent event) {}

void eos::GameState::resize(int width, int height) {}
