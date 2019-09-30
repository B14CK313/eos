//
// Created by jakob on 25.09.19.
//

#include <boost/assign.hpp>
#include "../../eos/GameEngine.hpp"
#include "menu/MenuState.hpp"
#include "ingame/IngameState.hpp"
#include "GameStates.hpp"

void game::initGameStates(std::shared_ptr<eos::GameEngine> gameEngine) {
    for(const auto& gameState : game::gameStates) gameState.second->init(gameEngine);
    gameEngine->stateManager.pushState(game::gameStates.at(game::GameState::MENU));
}

std::unordered_map<game::GameState, std::shared_ptr<eos::IGameState>> game::gameStates = boost::assign::map_list_of<game::GameState, std::shared_ptr<eos::IGameState>>
(game::GameState::MENU, std::make_shared<MenuState>())
(game::GameState::INGAME, std::make_shared<IngameState>());
