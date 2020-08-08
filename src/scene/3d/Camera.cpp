//
// Created by jakob on 19.07.20.
//

#include <glm/gtc/matrix_transform.hpp>
#include <eos/core/ServiceProvider.h>
#include "eos/scene/resources/Shader.h"
#include "eos/scene/3d/Camera.hpp"

eos::Camera::Camera(const glm::vec3& pos) : WorldObject{pos} {}

eos::Camera::Camera(const glm::vec3& pos, float yaw, float pitch, float roll) : WorldObject{pos, yaw, pitch, roll} {}

glm::mat4 eos::Camera::get_view_matrix() const {
    return glm::lookAt(pos_, pos_ + target_, up_);
}

void eos::Camera::apply_view_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders, const std::string_view name) const {
    for(const auto& shader : shaders) {
        shader->use();
        shader->set_mat4(name, get_view_matrix());
    }
}

glm::mat4 eos::Camera::get_projection_matrix(float zNear, float zFar) const {
    int windowWidth, windowHeight;
    eos::ServiceProvider::getWindow().get_size(windowWidth, windowHeight);
    return glm::perspectiveFov(fov_, static_cast<float>(windowWidth), static_cast<float>(windowHeight), zNear, zFar);
}

void eos::Camera::apply_projection_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders, const std::string_view name, float zNear,
                                          float zFar) const {
    glm::mat4 projectionMatrix = get_projection_matrix(zNear, zFar);
    for (const auto& shader : shaders) {
        shader->use();
        shader->set_mat4(name, projectionMatrix);
    }
}
