//
// Created by jakob on 13.07.20.
//

#ifndef EOS_SERVICEPROVIDER_H
#define EOS_SERVICEPROVIDER_H

#include "GameEngine.hpp"

namespace eos {

    class ServiceProvider {
    public:
        static eos::GameEngine* getEngine();

        static void provide(std::unique_ptr<eos::GameEngine> gameEngine);

        static GLFWwindow* getWindow();

        static void provide(GLFWwindow* window);

        static eos::StateManager* getStateManager();

        static void provide(std::unique_ptr<eos::StateManager> stateManager);

        static eos::Config* getConfig();

        static void provide(std::unique_ptr<Config> config);

    private:
        static std::unique_ptr<eos::GameEngine> gameEngine_;

        static GLFWwindow* window_;

        static std::unique_ptr<eos::StateManager> stateManager_;

        static std::unique_ptr<eos::Config> config_;

    };
}

#endif //EOS_SERVICEPROVIDER_H
