//
// Created by jakob on 03.08.20.
//

#ifndef EOS_PHYSICSOBJECT_HPP
#define EOS_PHYSICSOBJECT_HPP

#include <glm/glm.hpp>
#include "WorldObject.hpp"

namespace eos {

    class PhysicsObject : public WorldObject {
    public:
        explicit PhysicsObject(glm::vec3 pos, float mass);

        virtual void apply_force(glm::vec3 force);

        virtual void update(double t, double dt);

        float mass_;
        glm::vec3 acceleration_{0.0f};
        glm::vec3 velocity_{0.0f};
    };
}

#endif //EOS_PHYSICSOBJECT_HPP
