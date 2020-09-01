//
// Created by jakob on 13.07.20.
//

#include "eos/core/ServiceProvider.h"
#include <memory>
#include <spdlog/spdlog.h>

std::unique_ptr<eos::GameEngine> eos::ServiceProvider::gameEngine_;
std::unique_ptr<eos::StateManager> eos::ServiceProvider::stateManager_;
std::unique_ptr<eos::Config> eos::ServiceProvider::config_;
std::unique_ptr<eos::Window> eos::ServiceProvider::window_;
std::unique_ptr<FT_Library> eos::ServiceProvider::freetype_;

void eos::ServiceProvider::init(const std::string_view configPath, std::unique_ptr<eos::IGameState> initialState) {
    provide(std::make_unique<eos::Config>(configPath));
    provide(std::make_unique<eos::StateManager>(std::move(initialState)));
}

void eos::ServiceProvider::cleanup() {
    if(freetype_) FT_Done_FreeType(*freetype_);
}

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

FT_Library& eos::ServiceProvider::getFreetype() {
    if(!freetype_) {
        freetype_ = std::make_unique<FT_Library>();
        if(FT_Error error = FT_Init_FreeType(freetype_.get())){
            SPDLOG_ERROR("Could not initialize Freetype Library, error code {}", error);
        }
    }
    return *freetype_;
}
