//
// Created by jakob on 06.09.20.
//

#include <experimental/memory>
#include "eos/core/GameEngine.hpp"
#include "eos/scene/GameState.hpp"

void eos::GameState::init(std::experimental::observer_ptr<GameEngine> gameEngine) {
	gameEngine_ = gameEngine;
}

void eos::GameState::quit() {
	on_exit();
	gameEngine_->quit();
}

void eos::GameState::key_down(SDL_KeyboardEvent event) {}

void eos::GameState::key_up(SDL_KeyboardEvent event) {}

void eos::GameState::mouse_motion(SDL_MouseMotionEvent event) {}

void eos::GameState::mouse_button_down(SDL_MouseButtonEvent event) {}

void eos::GameState::mouse_button_up(SDL_MouseButtonEvent event) {}

void eos::GameState::mouse_wheel(SDL_MouseWheelEvent event) {}

void eos::GameState::resize(int width, int height) {}

