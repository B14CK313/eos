//
// Created by jakob on 25.09.19.
//

#ifndef GLFW_GAMEENGINE_GAMESTATES_HPP
#define GLFW_GAMEENGINE_GAMESTATES_HPP


#include <unordered_map>
#include "../../eos/IGameState.hpp"

namespace game {

    enum struct GameState {
        MENU, INGAME
    };

    extern std::unordered_map<GameState, std::shared_ptr<eos::IGameState>> gameStates;

    void initGameStates(std::shared_ptr<eos::GameEngine> gameEngine);

}

#endif //GLFW_GAMEENGINE_GAMESTATES_HPP
