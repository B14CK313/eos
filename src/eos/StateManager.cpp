//
// Created by jakob on 23.09.19.
//

#include <boost/log/trivial.hpp>
#include "StateManager.hpp"

void eos::StateManager::changeState(std::shared_ptr<IGameState> state) {
    if(!states.empty()) {
        states.back()->cleanup();
        states.pop_back();
    }

    states.push_back(state);
    states.back()->onEnter();
}

void eos::StateManager::pushState(std::shared_ptr<IGameState> state) {
    if(!states.empty()) states.back()->onExit();
    states.push_back(state);
    states.back()->onEnter();
}

void eos::StateManager::popState() {
    if(!states.empty()) {
        states.back()->cleanup();
        states.pop_back();

        if(!states.empty()){
            states.back()->onEnter();
            return;
        }
    }
    BOOST_LOG_TRIVIAL(error) << "No state on stack";
}

std::shared_ptr<eos::IGameState> eos::StateManager::currentState() {
    if (!states.empty()) return states.back();
    BOOST_LOG_TRIVIAL(error) << "No state on stack";
    return nullptr;
}
