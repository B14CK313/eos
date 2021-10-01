//
// Created by jakob on 10.09.19.
//

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <eos/core/graphics/Window.hpp>
#include "eos/scene/StateManager.hpp"
#include "eos/core/io/Config.hpp"

namespace eos {
	class GameEngine {
	public:
		explicit GameEngine(const std::string& configPath, std::unique_ptr<eos::GameState> initialState);

		bool run();

		void quit();

		void target_fps(int fps, bool cap = true);

		void target_ups(int ups);

		[[nodiscard]] int fps() const;

		[[nodiscard]] int ups() const;

		[[nodiscard]] std::experimental::observer_ptr<eos::Config> config() const;
		[[nodiscard]] std::experimental::observer_ptr<eos::Window> window() const;
		[[nodiscard]] std::experimental::observer_ptr<eos::StateManager> state_manager() const;

	private:
		std::unique_ptr<eos::Config> config_;

		std::unique_ptr<eos::Window> window_;

		std::unique_ptr<eos::StateManager> stateManager_;

		std::unique_ptr<FT_Library> freetypeLib_;

		bool quit_{false};

		double maxFrameTime_{};
		double dt_{};
		double fpu_{};

		int fps_;
		int ups_;
	};

}
