//
// Created by jakob on 23.09.19.
//

#include "../include/eos/StateManager.hpp"
#include <spdlog/spdlog.h>

void eos::StateManager::push_state(std::shared_ptr<IGameState> state) {
    if(!_stateStack.empty()) _stateStack.back()->on_exit();
    _stateStack.push_back(state);
    _stateStack.back()->on_enter();
}

void eos::StateManager::pop_state() {
    if(!_stateStack.empty()) {
        _stateStack.back()->on_exit();
        _stateStack.back()->cleanup();
        _stateStack.pop_back();

        if(!_stateStack.empty()){
            _stateStack.back()->on_enter();
            return;
        }
    }
    SPDLOG_ERROR("No state on stack");
}

std::shared_ptr<eos::IGameState> eos::StateManager::current_state() {
    if (!_stateStack.empty()) return _stateStack.back();
    SPDLOG_ERROR("No state on stack");
    return nullptr;
}
