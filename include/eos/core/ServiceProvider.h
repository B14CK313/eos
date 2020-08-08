//
// Created by jakob on 13.07.20.
//

#ifndef EOS_SERVICEPROVIDER_H
#define EOS_SERVICEPROVIDER_H

#include "eos/core/GameEngine.hpp"
#include "eos/scene/Window.h"

namespace eos {

    class ServiceProvider {
    public:
        static void init(const std::string_view configPath, std::unique_ptr<eos::IGameState> initialState);


        static void provide(std::unique_ptr<eos::GameEngine> gameEngine);

        static eos::GameEngine& getGameEngine();


        static void provide(std::unique_ptr<eos::StateManager> stateManager);

        static eos::StateManager& getStateManager();


        static void provide(std::unique_ptr<eos::Config> config);

        static eos::Config& getConfig();


        static void provide(std::unique_ptr<eos::Window> window);

        static eos::Window& getWindow();

    private:
        static std::unique_ptr<eos::GameEngine> gameEngine_;

        static std::unique_ptr<eos::StateManager> stateManager_;

        static std::unique_ptr<eos::Config> config_;

        static std::unique_ptr<eos::Window> window_;

    };
}

#endif //EOS_SERVICEPROVIDER_H
