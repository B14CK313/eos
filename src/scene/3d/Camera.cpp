//
// Created by jakob on 19.07.20.
//

#include <glm/gtc/matrix_transform.hpp>
#include "eos/scene/resources/Shader.hpp"
#include "eos/core/graphics/Graphics.hpp"
#include "eos/scene/3d/Camera.hpp"

eos::Camera::Camera(const glm::vec3& pos) : WorldObject{pos} {}

eos::Camera::Camera(const glm::vec3& pos, float yaw, float pitch, float roll) : WorldObject{pos, yaw, pitch, roll} {}

void eos::Camera::init(std::experimental::observer_ptr<eos::Graphics> graphics) {
	graphics_ = graphics;
}

glm::mat4 eos::Camera::get_view_matrix() const {
	return glm::lookAt(pos_, pos_ + target_, up_);
}

void eos::Camera::apply_view_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders,
                                    const std::string& name) const {
	for (const auto& shader: shaders) {
		shader->use();
		shader->set_mat4_uniform(name, get_view_matrix());
	}
}

glm::mat4 eos::Camera::get_projection_matrix(float zNear, float zFar) const {
	const auto& windowSize = graphics_->size();
	return glm::perspectiveFov(fov_, static_cast<float>(windowSize.x), static_cast<float>(windowSize.y), zNear, zFar);
}

void eos::Camera::apply_projection_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders,
                                          const std::string& name, float zNear,
                                          float zFar) const {
	glm::mat4 projectionMatrix = get_projection_matrix(zNear, zFar);
	for (const auto& shader: shaders) {
		shader->use();
		shader->set_mat4_uniform(name, projectionMatrix);
	}
}
