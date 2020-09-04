//
// Created by jakob on 16.07.20.
//

#include <glm/gtc/matrix_transform.hpp>
#include <eos/core/ServiceProvider.h>
#include <eos/scene/resources/Shader.h>
#include "eos/scene/3d/ArcballCamera.h"



eos::ArcballCamera::ArcballCamera(glm::vec3 pos, float yaw, float pitch, glm::vec3 worldUp) : pos_{pos}, yaw_{yaw}, pitch_{pitch}, worldUp_{worldUp} {
    update_vectors();
}

eos::ArcballCamera::ArcballCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 worldUp) : pos_{pos}, front_{front}, worldUp_{worldUp} {
    update_vectors();
}

eos::ArcballCamera::ArcballCamera(glm::vec3 pos, float speed, float sensitivity, float zoom) : pos_{pos}, speed_{speed}, sensitivity_{sensitivity}, zoom_{zoom} {
    update_vectors();
}

glm::mat4 eos::ArcballCamera::get_view_matrix() const {
    return glm::lookAt(pos_, pos_ + front_, up_);
}

void eos::ArcballCamera::apply_view_matrix(const eos::Shader& shader, const std::string& name) const {
    shader.set_mat4_uniform(name, get_view_matrix());
}

glm::mat4 eos::ArcballCamera::get_projection_matrix(float zNear, float zFar) const {
    int windowWidth, windowHeight;
    eos::ServiceProvider::getWindow().get_size(windowWidth, windowHeight);
    return glm::perspective(glm::radians(zoom_), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), zNear, zFar);
}

void eos::ArcballCamera::apply_projection_matrix(const eos::Shader& shader, const std::string& name, float zNear,
                                                 float zFar) const {
    shader.set_mat4_uniform(name, get_projection_matrix(zNear, zFar));
}

void eos::ArcballCamera::key_input(eos::ArcballCamera::Movement dir) {
    if (dir == Movement::FORWARD) pos_ += front_ * speed_;
    if (dir == Movement::BACKWARD) pos_ -= front_ * speed_;
    if (dir == Movement::LEFT) pos_ -= right_ * speed_;
    if (dir == Movement::RIGHT) pos_ += right_ * speed_;
}

void eos::ArcballCamera::mouse_input(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= sensitivity_;
    yOffset *= sensitivity_;

    yaw_ += xOffset;
    pitch_ += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    update_vectors();
}

void eos::ArcballCamera::scroll_input(float yOffset) {
    zoom_ -= yOffset;
    if (zoom_ < 1.0f) zoom_ = 1.0f;
    if (zoom_ > 45.0f) zoom_ = 45.0f;
}

void eos::ArcballCamera::update_vectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
