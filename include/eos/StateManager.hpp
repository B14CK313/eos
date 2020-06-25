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
        void push_state(std::shared_ptr<IGameState> state);

        void pop_state();

        std::shared_ptr<IGameState> current_state();
    private:
        std::vector<std::shared_ptr<IGameState>> stateStack_;
    };

}

#endif //EOS_STATEMANAGER_HPP
