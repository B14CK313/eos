//
// Created by jakob on 19.07.20.
//

#include <glm/gtc/matrix_transform.hpp>
#include <eos/ServiceProvider.h>
#include "eos/Shader.h"
#include "eos/Camera.hpp"

eos::Camera::Camera(const glm::vec3& pos) : pos_{pos} {}

eos::Camera::Camera(const glm::vec3& pos, float yaw, float pitch) : pos_{pos}, yaw_{yaw}, pitch_{pitch} {}

eos::Camera& eos::Camera::move(eos::Direction dir) {
    if(dir == eos::Direction::FORWARD) move_forward(1.0f);
    else if(dir == eos::Direction::BACKWARD) move_backward(1.0f);
    else if(dir == eos::Direction::RIGHT) move_right(1.0f);
    else if(dir == eos::Direction::LEFT) move_left(1.0f);
    else if(dir == eos::Direction::UP) move_up(1.0f);
    else if(dir == eos::Direction::DOWN) move_down(1.0f);
    return *this;
}

eos::Camera& eos::Camera::move(const glm::vec3& dir) {
    glm::vec3 offset;
    offset += dir.x * glm::normalize(glm::cross(target_, up_));
    offset.y += dir.y;
    offset += dir.z * glm::normalize(glm::vec3{target_.x, 0.0f, target_.z});
    pos_ += glm::normalize(offset);
    recalculate_vectors();
    return *this;
}

eos::Camera& eos::Camera::move_long(float dist) {
    pos_ += dist * glm::normalize(glm::vec3{target_.x, 0.0f, target_.z});
    recalculate_vectors();
    return *this;
}

eos::Camera& eos::Camera::move_forward(float dist) {
    return move_long(dist);
}

eos::Camera& eos::Camera::move_backward(float dist) {
    return move_forward(-dist);
}

eos::Camera& eos::Camera::move_lat(float dist) {
    pos_ += dist * glm::normalize(glm::cross(target_, up_));
    recalculate_vectors();
    return *this;
}

eos::Camera& eos::Camera::move_right(float dist) {
    return move_lat(dist);
}

eos::Camera& eos::Camera::move_left(float dist) {
    return move_right(-dist);
}

eos::Camera& eos::Camera::move_vert(float dist) {
    pos_.y += dist;
    recalculate_vectors();
    return *this;
}

eos::Camera& eos::Camera::move_up(float dist) {
    return move_vert(dist);
}

eos::Camera& eos::Camera::move_down(float dist) {
    return move_up(-dist);
}

eos::Camera& eos::Camera::teleport(const glm::vec3& pos) {
    pos_ = pos;
    recalculate_vectors();
    return *this;
}

eos::Camera& eos::Camera::rotate(float deltaX, float deltaY, bool constrainPitch) {
    yaw_ += deltaX * 0.1f;
    pitch_ += deltaY * 0.1f;

    if (constrainPitch) {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    recalculate_vectors();
    return *this;
}

glm::mat4 eos::Camera::get_view_matrix() const {
    return glm::lookAt(pos_, pos_ + target_, up_);
}

void eos::Camera::apply_view_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders, const std::string& name) const {
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

void eos::Camera::apply_projection_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders, const std::string& name, float zNear,
                                          float zFar) const {
    glm::mat4 projectionMatrix = get_projection_matrix(zNear, zFar);
    for (const auto& shader : shaders) {
        shader->use();
        shader->set_mat4(name, projectionMatrix);
    }
}

void eos::Camera::recalculate_vectors() {
    glm::vec3 target;
    target.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    target.y = sin(glm::radians(pitch_));
    target.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    target_ = glm::normalize(target);
}
