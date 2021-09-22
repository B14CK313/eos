//
// Created by jakob on 23.09.19.
//

#pragma once

#include <vector>
#include "GameState.hpp"

namespace eos {

    class StateManager {
    public:
        explicit StateManager(std::unique_ptr<GameState> initialState);

        void push_state(std::unique_ptr<GameState> state);

        void pop_state();

        eos::GameState* getState();

    private:
        std::vector<std::unique_ptr<GameState>> stateStack_;
    };

}
