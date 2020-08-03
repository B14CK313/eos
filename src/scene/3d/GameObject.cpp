//
// Created by jakob on 19.07.20.
//

#include "eos/scene/3d/GameObject.h"

eos::GameObject::GameObject(glm::vec3 pos, float mass) : pos_{pos}, mass_{mass} {

}

void eos::GameObject::apply_force(glm::vec3 force) {
    //acceleration_ += force / mass_;
}

void eos::GameObject::update(double t, double dt) {
    //velocity_ += acceleration_ * static_cast<float>(dt);
    //pos_ += velocity_ * static_cast<float>(dt);
}

void eos::GameObject::render(double interpolation) {

}
