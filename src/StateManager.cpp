//
// Created by jakob on 23.09.19.
//

#include "../include/eos/StateManager.hpp"
#include <spdlog/spdlog.h>

eos::StateManager::StateManager(std::unique_ptr<IGameState> initialState) {
    push_state(std::move(initialState));
}

void eos::StateManager::push_state(std::unique_ptr<IGameState> state) {
    if (!stateStack_.empty()) stateStack_.back()->on_exit();
    stateStack_.push_back(std::move(state));
    stateStack_.back()->on_enter();
}

void eos::StateManager::pop_state() {
    if (!stateStack_.empty()) {
        stateStack_.back()->on_exit();
        stateStack_.pop_back();

        if (!stateStack_.empty()) {
            stateStack_.back()->on_enter();
            return;
        }
    }
    SPDLOG_ERROR("No state on stack");
}

eos::IGameState* eos::StateManager::getState() {
    if (!stateStack_.empty()) {
        return stateStack_.back().get();
    } else {
        SPDLOG_ERROR("No state on stack");
        return nullptr;
    }
}
