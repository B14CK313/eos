#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility//setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "eos/GameEngine.hpp"
#include "game/states/GameStates.hpp"


void init() {
    boost::log::add_console_log(std::cout);
#ifdef NDEBUG
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif //_DEBUG
    boost::log::add_common_attributes();
    BOOST_LOG_TRIVIAL(trace) << "Logger initialized";
}

int main(int argc, char* argv[]) {
    init();

    BOOST_LOG_TRIVIAL(trace) << "Initializing GameEngine, pushing first GameStates";
    auto gameEngine = std::make_shared<eos::GameEngine>("game.conf");
    game::initGameStates(gameEngine);
    BOOST_LOG_TRIVIAL(trace) << "Starting game loop...";
    return gameEngine->run();
}