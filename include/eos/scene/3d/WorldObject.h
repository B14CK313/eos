//
// Created by jakob on 12.07.20.
//

#ifndef EOS_GAMEOBJECT_H
#define EOS_GAMEOBJECT_H

#include <glm/glm.hpp>
#include <eos/core/ColorRGB.hpp>
#include "eos/Constants.hpp"

namespace eos {

    class WorldObject {
    public:
        glm::vec3 pos_;
        glm::vec3 target_{0.0f, 0.0f, -1.0f};
        glm::vec3 up_{0.0f, 1.0f, 0.0f};

        float pitch_{0.0f}; // Rotation around x-axis (up/down)
        float yaw_{-90.0f}; // Rotation around y-axis (left/right)
        float roll_{0.0f}; // Rotation around z-axis

        ColorRGB color_;

        explicit WorldObject(glm::vec3 pos);

        WorldObject(glm::vec3 pos, float yaw, float pitch, float roll);

        virtual ~WorldObject() = default;

        WorldObject& move(eos::Direction dir);

        WorldObject& move(const glm::vec3& dir);

        WorldObject& move_long(float dist);

        WorldObject& move_lat(float dist);

        WorldObject& move_vert(float dist);

        WorldObject& teleport(const glm::vec3& pos);

        WorldObject& rotate(float deltaX, float deltaY, bool constrainPitch = true);

        virtual void render(double interpolation);

    private:
        void recalculate_vectors();
    };

}

#endif //EOS_GAMEOBJECT_H
