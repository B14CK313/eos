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
        void changeState(const std::shared_ptr<IGameState>& state);

        void pushState(const std::shared_ptr<IGameState>& state);

        void popState();

        std::shared_ptr<IGameState> currentState();

    private:
        std::vector<std::shared_ptr<IGameState>> states;
    };

}

#endif //EOS_STATEMANAGER_HPP