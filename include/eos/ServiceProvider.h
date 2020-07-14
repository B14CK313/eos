//
// Created by jakob on 13.07.20.
//

#ifndef EOS_SERVICEPROVIDER_H
#define EOS_SERVICEPROVIDER_H

#include "GameEngine.hpp"
#include "Window.h"

namespace eos {

    class ServiceProvider {
    public:
        static void provide(std::unique_ptr<eos::GameEngine> gameEngine);

        static eos::GameEngine& getGameEngine();


        static void provide(std::unique_ptr<eos::StateManager> stateManager);

        static eos::StateManager& getStateManager();


        static void provide(std::unique_ptr<eos::Config> config);

        static eos::Config& getConfig();


        static void provide(std::unique_ptr<eos::Window> window);

        static eos::Window& getWindow();

    private:
        inline static std::unique_ptr<eos::GameEngine> gameEngine_;

        inline static std::unique_ptr<eos::StateManager> stateManager_;

        inline static std::unique_ptr<eos::Config> config_;

        inline static std::unique_ptr<eos::Window> window_;

    };
}

#endif //EOS_SERVICEPROVIDER_H
