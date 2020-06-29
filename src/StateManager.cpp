//
// Created by jakob on 23.09.19.
//

#include "../include/eos/StateManager.hpp"
#include <spdlog/spdlog.h>

void eos::StateManager::push_state(std::shared_ptr<IGameState> state) {
    if(!stateStack_.empty()) stateStack_.back()->on_exit();
    stateStack_.push_back(state);
    stateStack_.back()->on_enter();
}

void eos::StateManager::pop_state() {
    if(!stateStack_.empty()) {
        stateStack_.back()->on_exit();
        stateStack_.pop_back();

        if(!stateStack_.empty()){
            stateStack_.back()->on_enter();
            return;
        }
    }
    SPDLOG_ERROR("No state on stack");
}

std::shared_ptr<eos::IGameState> eos::StateManager::current_state() {
    if (!stateStack_.empty()) return stateStack_.back();
    SPDLOG_ERROR("No state on stack");
    return nullptr;
}
