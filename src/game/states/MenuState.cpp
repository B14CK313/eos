//
// Created by jakob on 24.09.19.
//

#include <glbinding/gl/gl.h>
#include <GLFW/glfw3.h>
#include <boost/log/trivial.hpp>
#include "../../eos/GameEngine.hpp"
#include "GameStates.hpp"
#include "MenuState.hpp"

bool game::MenuState::init(std::shared_ptr<eos::GameEngine> gameEngine) {
    this->gameEngine = gameEngine;
    return true;
}

bool game::MenuState::cleanup() {
    return true;
}

void game::MenuState::onEnter() {
    glClearColor(1, 1, 1, 1);
}

void game::MenuState::onExit() {

}

void game::MenuState::resize(int width, int height) {

}

void game::MenuState::input(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(gameEngine->window, GLFW_TRUE);
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        gameEngine->stateManager.pushState(game::gameStates[game::GameState::INGAME]);
}

void game::MenuState::update(double t, double dt) {

}

void game::MenuState::render(double interpolation) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(gameEngine->window);
}
