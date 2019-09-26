//
// Created by jakob on 24.09.19.
//

#include <boost/log/trivial.hpp>
#include <GLFW/glfw3.h>
#include "../../eos/GameEngine.hpp"
#include "IngameState.hpp"

bool game::IngameState::init(std::shared_ptr<eos::GameEngine> gameEngine) {
    this->gameEngine = gameEngine;
    return true;
}

bool game::IngameState::cleanup() {
    return true;
}

void game::IngameState::onEnter() {
    glClearColor(0,0,0,1);
}

void game::IngameState::onExit() {

}

void game::IngameState::resize(int width, int height){

}

void game::IngameState::input(int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(gameEngine->window, GLFW_TRUE);
    else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) gameEngine->stateManager.popState();
}

void game::IngameState::update(double t, double dt) {

}

void game::IngameState::render(double interpolation) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(gameEngine->window);
}