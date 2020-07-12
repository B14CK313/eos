//
// Created by jakob on 23.09.19.
//

#include "../include/eos/StateManager.hpp"
#include <spdlog/spdlog.h>

void eos::StateManager::push_state(std::shared_ptr<IGameState> state) {
    if (!stateStack_.empty()) stateStack_.back()->on_exit();
    stateStack_.push_back(state);
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

void eos::StateManager::resize(int width, int height) {
    if (!stateStack_.empty()) {
        stateStack_.back()->resize(width, height);
    } else {
        SPDLOG_ERROR("No state on stack");
    }
}

void eos::StateManager::input(int key, int scancode, int action, int mods) {
    if (!stateStack_.empty()) {
        stateStack_.back()->input(key, scancode, action, mods);
    } else {
        SPDLOG_ERROR("No state on stack");
    }
}

void eos::StateManager::update(double t, double dt) {
    if (!stateStack_.empty()) {
        stateStack_.back()->update(t, dt);
    } else {
        SPDLOG_ERROR("No state on stack");
    }
}

void eos::StateManager::render(double interpolation) {
    if (!stateStack_.empty()) {
        stateStack_.back()->render(interpolation);
    } else {
        SPDLOG_ERROR("No state on stack");
    }
}
