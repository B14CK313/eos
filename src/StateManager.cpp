//
// Created by jakob on 23.09.19.
//

#include "../include/eos/StateManager.hpp"
#include <spdlog/spdlog.h>

void eos::StateManager::changeState(const std::shared_ptr<IGameState>& state) {
    if(!states.empty()) {
        states.back()->cleanup();
        states.pop_back();
    }

    states.push_back(state);
    states.back()->onEnter();
}

void eos::StateManager::pushState(const std::shared_ptr<IGameState>& state) {
    if(!states.empty()) states.back()->onExit();
    states.push_back(state);
    states.back()->onEnter();
}

void eos::StateManager::popState() {
    if(!states.empty()) {
        states.back()->onExit();
        states.back()->cleanup();
        states.pop_back();

        if(!states.empty()){
            states.back()->onEnter();
            return;
        }
    }
    SPDLOG_ERROR("No state on stack");
}

std::shared_ptr<eos::IGameState> eos::StateManager::currentState() {
    if (!states.empty()) return states.back();
    SPDLOG_ERROR("No state on stack");
    return nullptr;
}
