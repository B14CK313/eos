//
// Created by jakob on 23.09.19.
//

#ifndef EOS_STATEMANAGER_HPP
#define EOS_STATEMANAGER_HPP

#include <vector>
#include "IGameState.hpp"

namespace eos {

    class StateManager {
    public:
        explicit StateManager(std::unique_ptr<IGameState> initialState);

        void push_state(std::unique_ptr<IGameState> state);

        void pop_state();

        eos::IGameState* getState();

    private:
        std::vector<std::unique_ptr<IGameState>> stateStack_;
    };

}

#endif //EOS_STATEMANAGER_HPP
