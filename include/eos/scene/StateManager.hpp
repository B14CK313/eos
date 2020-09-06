//
// Created by jakob on 23.09.19.
//

#ifndef EOS_STATEMANAGER_HPP
#define EOS_STATEMANAGER_HPP

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

#endif //EOS_STATEMANAGER_HPP
