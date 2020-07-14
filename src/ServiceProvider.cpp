//
// Created by jakob on 13.07.20.
//

#include "eos/ServiceProvider.h"
#include <memory>
#include <spdlog/spdlog.h>

void eos::ServiceProvider::provide(std::unique_ptr<eos::GameEngine> gameEngine) {
    gameEngine_ = std::move(gameEngine);
}

eos::GameEngine& eos::ServiceProvider::getGameEngine() {
    if(!gameEngine_) {
        gameEngine_ = std::make_unique<eos::GameEngine>();
    }
    return *gameEngine_;
}


void eos::ServiceProvider::provide(std::unique_ptr<eos::StateManager> stateManager) {
    stateManager_ = std::move(stateManager);
}

eos::StateManager& eos::ServiceProvider::getStateManager() {
    if(!stateManager_) {
        SPDLOG_CRITICAL("No StateManager was provided!");
    }
    return *stateManager_;
}


void eos::ServiceProvider::provide(std::unique_ptr<eos::Config> config) {
    config_ = std::move(config);
}

eos::Config& eos::ServiceProvider::getConfig() {
    if(!config_){
        SPDLOG_CRITICAL("No Config was provided!");
    }
    return *config_;
}


void eos::ServiceProvider::provide(std::unique_ptr<eos::Window> window) {
    window_ = std::move(window);
}

eos::Window& eos::ServiceProvider::getWindow() {
    if(!window_) {
        if(!gameEngine_) {
            getGameEngine();
        }
    }
    return *window_;
}