//
// Created by jakob on 19.07.20.
//

#include "eos/scene/3d/WorldObject.hpp"

eos::WorldObject::WorldObject(glm::vec3 pos) : pos_{pos} {

}

eos::WorldObject::WorldObject(glm::vec3 pos, float yaw, float pitch, float roll) : pos_{pos}, yaw_{yaw}, pitch_{pitch}, roll_{roll} {

}

eos::WorldObject& eos::WorldObject::move(eos::Direction dir) {
    if(dir == eos::Direction::FORWARD) move_long(1.0f);
    else if(dir == eos::Direction::BACKWARD) move_long(-1.0f);
    else if(dir == eos::Direction::RIGHT) move_lat(1.0f);
    else if(dir == eos::Direction::LEFT) move_lat(-1.0f);
    else if(dir == eos::Direction::UP) move_vert(1.0f);
    else if(dir == eos::Direction::DOWN) move_vert(-1.0f);
    return *this;
}

eos::WorldObject& eos::WorldObject::move(const glm::vec3& dir) {
    glm::vec3 offset;
    offset += dir.x * glm::normalize(glm::cross(target_, up_));
    offset.y += dir.y;
    offset += dir.z * glm::normalize(glm::vec3{target_.x, 0.0f, target_.z});
    pos_ += glm::normalize(offset);
    recalculate_vectors();
    return *this;
}

eos::WorldObject& eos::WorldObject::move_long(float dist) {
    pos_ += dist * glm::normalize(glm::vec3{target_.x, 0.0f, target_.z});
    recalculate_vectors();
    return *this;
}

eos::WorldObject& eos::WorldObject::move_lat(float dist) {
    pos_ += dist * glm::normalize(glm::cross(target_, up_));
    recalculate_vectors();
    return *this;
}

eos::WorldObject& eos::WorldObject::move_vert(float dist) {
    pos_.y += dist;
    recalculate_vectors();
    return *this;
}

eos::WorldObject& eos::WorldObject::teleport(const glm::vec3& pos) {
    pos_ = pos;
    recalculate_vectors();
    return *this;
}

eos::WorldObject& eos::WorldObject::rotate(float deltaX, float deltaY, bool constrainPitch) {
    yaw_ += deltaX;
    pitch_ += deltaY;

    if (constrainPitch) {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    recalculate_vectors();
    return *this;
}

void eos::WorldObject::render(double interpolation) {

}

void eos::WorldObject::recalculate_vectors() {
    glm::vec3 target;
    target.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    target.y = sin(glm::radians(pitch_));
    target.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    target_ = glm::normalize(target);
}
