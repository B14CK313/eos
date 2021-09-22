//
// Created by jakob on 10.09.21.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <eos/core/ServiceProvider.hpp>

class TestGameState : public eos::GameState{
	void on_enter() override {
		std::cout << "Enter";
	}

	void on_exit() override {
		std::cout << "Exit";
	}

	void update(double t, double dt) override {

	}

	void render(double interpolation) override {

	}
};

TEST_CASE("Simple startup test") {
	eos::ServiceProvider::init("", std::make_unique<TestGameState>());
	eos::ServiceProvider::getGameEngine().run();
}