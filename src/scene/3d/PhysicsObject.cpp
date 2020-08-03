//
// Created by jakob on 03.08.20.
//

#include "eos/scene/3d/PhysicsObject.hpp"

eos::PhysicsObject::PhysicsObject(glm::vec3 pos, float mass) : WorldObject{pos}, mass_{mass}{

}

void eos::PhysicsObject::apply_force(glm::vec3 force) {

}

void eos::PhysicsObject::update(double t, double dt) {

}
