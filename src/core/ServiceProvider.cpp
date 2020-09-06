//
// Created by jakob on 13.07.20.
//

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "eos/core/ServiceProvider.h"

std::shared_ptr<eos::GameEngine> eos::ServiceProvider::gameEngine_;
std::shared_ptr<eos::StateManager> eos::ServiceProvider::stateManager_;
std::shared_ptr<eos::Config> eos::ServiceProvider::config_;
std::shared_ptr<eos::Window> eos::ServiceProvider::window_;
std::shared_ptr<FT_Library> eos::ServiceProvider::freetype_;

void eos::ServiceProvider::init(const std::string& configPath, std::unique_ptr<eos::GameState> initialState) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks[0]->set_level(spdlog::level::trace);

    config_ = std::make_shared<eos::Config>(configPath);

    sinks.push_back(
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(config_->log.fileName, true));
    sinks[1]->set_level(spdlog::level::warn);

    auto defaultLogger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
    defaultLogger->set_level(spdlog::level::trace);
    defaultLogger->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] [%s:%#] %v");
    spdlog::register_logger(defaultLogger);
    spdlog::set_default_logger(defaultLogger);
    SPDLOG_TRACE("Logger initialized");

    window_ = std::make_shared<eos::Window>(config_->window.title, config_->window.width, config_->window.height);
    stateManager_ = std::make_shared<eos::StateManager>(std::move(initialState));
    gameEngine_ = std::make_shared<eos::GameEngine>();
}

void eos::ServiceProvider::cleanup() {
    if(freetype_) FT_Done_FreeType(*freetype_);
}

void eos::ServiceProvider::provide(std::shared_ptr<eos::GameEngine> gameEngine) {
    gameEngine_ = gameEngine;
}

eos::GameEngine& eos::ServiceProvider::getGameEngine() {
    if(!gameEngine_) {
        throw;
    }
    return *gameEngine_;
}

std::shared_ptr<eos::GameEngine> eos::ServiceProvider::getGameEnginePtr() {
    return gameEngine_;
}

void eos::ServiceProvider::provide(std::shared_ptr<eos::StateManager> stateManager) {
    stateManager_ = stateManager;
}

eos::StateManager& eos::ServiceProvider::getStateManager() {
    if(!stateManager_) {
        SPDLOG_CRITICAL("No StateManager was provided!");
    }
    return *stateManager_;
}

std::shared_ptr<eos::StateManager> eos::ServiceProvider::getStateManagerPtr() {
    return stateManager_;
}

void eos::ServiceProvider::provide(std::shared_ptr<eos::Config> config) {
    config_ = config;
}

eos::Config& eos::ServiceProvider::getConfig() {
    if(!config_){
        SPDLOG_CRITICAL("No Config was provided!");
    }
    return *config_;
}

std::shared_ptr<eos::Config> eos::ServiceProvider::getConfigPtr() {
    return config_;
}

void eos::ServiceProvider::provide(std::shared_ptr<eos::Window> window) {
    window_ = window;
}

eos::Window& eos::ServiceProvider::getWindow() {
    if(!window_) {
        throw;
    }
    return *window_;
}

std::shared_ptr<eos::Window> eos::ServiceProvider::getWindowPtr() {
    return window_;
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
