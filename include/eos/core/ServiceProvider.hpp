//
// Created by jakob on 13.07.20.
//

#ifndef EOS_SERVICEPROVIDER_HPP
#define EOS_SERVICEPROVIDER_HPP

#include <ft2build.h>
#include FT_FREETYPE_H
#include "eos/core/GameEngine.hpp"
#include "eos/scene/Window.hpp"

namespace eos {

    class ServiceProvider {
    public:
        static void init(const std::string& configPath, std::unique_ptr<eos::GameState> initialState);

        static void cleanup();


        static void provide(std::shared_ptr<eos::GameEngine> gameEngine);

        static eos::GameEngine& getGameEngine();

        static std::shared_ptr<eos::GameEngine> getGameEnginePtr();


        static void provide(std::shared_ptr<eos::StateManager> stateManager);

        static eos::StateManager& getStateManager();

        static std::shared_ptr<eos::StateManager> getStateManagerPtr();


        static void provide(std::shared_ptr<eos::Config> config);

        static eos::Config& getConfig();

        static std::shared_ptr<eos::Config> getConfigPtr();


        static void provide(std::shared_ptr<eos::Window> window);

        static eos::Window& getWindow();

        static std::shared_ptr<eos::Window> getWindowPtr();


        static FT_Library& getFreetype();

    private:
        static std::shared_ptr<eos::GameEngine> gameEngine_;

        static std::shared_ptr<eos::StateManager> stateManager_;

        static std::shared_ptr<eos::Config> config_;

        static std::shared_ptr<eos::Window> window_;

        static std::shared_ptr<FT_Library> freetype_;
    };
}

#endif //EOS_SERVICEPROVIDER_HPP
