//
// Created by jakob on 13.07.20.
//

#include "eos/ServiceProvider.h"
#include <memory>

std::unique_ptr<eos::GameEngine> eos::ServiceProvider::gameEngine_;

eos::GameEngine* eos::ServiceProvider::getEngine() {
    return gameEngine_.get();
}

void eos::ServiceProvider::provide(std::unique_ptr<eos::GameEngine> gameEngine) {
    gameEngine_ = std::move(gameEngine);
}

GLFWwindow* eos::ServiceProvider::window_;

GLFWwindow* eos::ServiceProvider::getWindow() {
    return window_;
}

void eos::ServiceProvider::provide(GLFWwindow* window) {
    window_ = window;
}

std::unique_ptr<eos::StateManager> eos::ServiceProvider::stateManager_;

eos::StateManager* eos::ServiceProvider::getStateManager() {
    return stateManager_.get();
}

void eos::ServiceProvider::provide(std::unique_ptr<eos::StateManager> stateManager) {
    stateManager_ = std::move(stateManager);
}

std::unique_ptr<eos::Config> eos::ServiceProvider::config_;

eos::Config* eos::ServiceProvider::getConfig() {
    return config_.get();
}

void eos::ServiceProvider::provide(std::unique_ptr<Config> config) {
    config_ = std::move(config);
}
