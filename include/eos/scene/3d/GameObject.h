//
// Created by jakob on 12.07.20.
//

#ifndef EOS_GAMEOBJECT_H
#define EOS_GAMEOBJECT_H

#include <glm/glm.hpp>

namespace eos {

    class GameObject {
    public:
        GameObject(glm::vec3 pos, float mass);

        virtual ~GameObject() = default;

        virtual void apply_force(glm::vec3 force);

        virtual void update(double t, double dt);

        virtual void render(double interpolation);

        glm::vec3 pos_;
        glm::vec3 acceleration_{0.0f};
        glm::vec3 velocity_{0.0f};

        float mass_;
    };

}

#endif //EOS_GAMEOBJECT_H
