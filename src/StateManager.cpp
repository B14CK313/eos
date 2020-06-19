//
// Created by jakob on 23.09.19.
//

#include "../include/eos/StateManager.hpp"
#include <spdlog/spdlog.h>

void eos::StateManager::pushState(std::shared_ptr<IGameState> state) {
    if(!state_stack.empty()) state_stack.back()->onExit();
    state_stack.push_back(state);
    state_stack.back()->onEnter();
}

void eos::StateManager::popState() {
    if(!state_stack.empty()) {
        state_stack.back()->onExit();
        state_stack.back()->cleanup();
        state_stack.pop_back();

        if(!state_stack.empty()){
            state_stack.back()->onEnter();
            return;
        }
    }
    SPDLOG_ERROR("No state on stack");
}

std::shared_ptr<eos::IGameState> eos::StateManager::currentState() {
    if (!state_stack.empty()) return state_stack.back();
    SPDLOG_ERROR("No state on stack");
    return nullptr;
}
