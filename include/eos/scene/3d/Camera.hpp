//
// Created by jakob on 19.07.20.
//

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <experimental/memory>
#include "WorldObject.hpp"

namespace eos {

	class Shader;

	class Graphics;

	class Camera : public WorldObject {
	public:
		float fov_{glm::radians(90.0f)};

		explicit Camera(const glm::vec3& pos = {0.0f, 0.0f, 0.0f});

		Camera(const glm::vec3& pos, float yaw, float pitch, float roll = 0.0f);

		void init(std::experimental::observer_ptr<eos::Graphics> graphics);

		[[nodiscard]] glm::mat4 get_projection_matrix(float zNear = 0.1f, float zFar = 100.0f) const;

		[[nodiscard]] glm::mat4 get_view_matrix() const;

		void apply_view_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders,
		                       const std::string& name = "view") const;

		void apply_projection_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders,
		                             const std::string& name = "projection", float zNear = 0.1f,
		                             float zFar = 100.0f) const;

	private:
		std::experimental::observer_ptr<Graphics> graphics_;
	};
}
