//
// Created by jakob on 23.09.19.
//

#include <spdlog/spdlog.h>
#include <experimental/memory>
#include "eos/scene/StateManager.hpp"

eos::StateManager::StateManager(std::unique_ptr<GameState> initialState) {
	push_state(std::move(initialState));
}

void eos::StateManager::push_state(std::unique_ptr<GameState> state) {
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

std::experimental::observer_ptr<eos::GameState> eos::StateManager::get_state() {
	if (!stateStack_.empty()) {
		return std::experimental::make_observer(stateStack_.back().get());
	} else {
		SPDLOG_ERROR("No state on stack");
		return nullptr;
	}
}
