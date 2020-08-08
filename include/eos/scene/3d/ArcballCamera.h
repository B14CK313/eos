//
// Created by jakob on 16.07.20.
//

#ifndef EOS_ARCBALL_CAMERA_H
#define EOS_ARCBALL_CAMERA_H

#include <glm/glm.hpp>

namespace eos {

    class ArcballCamera {
    private:
        // Default camera values
        constexpr static const glm::vec3 FRONT{0.0f, 0.0f, -1.0f};
        constexpr static const glm::vec3 WORLD_UP{0.0f, 1.0f, 0.0f};
        constexpr static const float YAW = -90.0f;
        constexpr static const float PITCH = 0.0f;
        constexpr static const float SPEED = 2.5f;
        constexpr static const float SENSITIVITY = 0.1f;
        constexpr static const float ZOOM = 45.0f;

    public:
        enum struct Movement {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT
        };

        glm::vec3 pos_;
        glm::vec3 front_{FRONT};
        glm::vec3 worldUp_{WORLD_UP};
        glm::vec3 up_;
        glm::vec3 right_;

        float yaw_{YAW};
        float pitch_{PITCH};

        float speed_{SPEED};
        float sensitivity_{SENSITIVITY};
        float zoom_{ZOOM};

        explicit ArcballCamera(glm::vec3 pos = {0.0f, 0.0f, 0.0f}, float yaw = YAW, float pitch = PITCH, glm::vec3 worldUp = WORLD_UP);

        explicit ArcballCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 worldUp = WORLD_UP);

        ArcballCamera(glm::vec3 pos, float speed, float sensitivity = SENSITIVITY, float zoom = ZOOM);

        [[nodiscard]] glm::mat4 get_view_matrix() const;

        void apply_view_matrix(const eos::Shader& shader, const std::string_view name = "view") const;

        [[nodiscard]] glm::mat4 get_projection_matrix(float zNear = 0.1f, float zFar = 100.0f) const;

        void apply_projection_matrix(const eos::Shader& shader, const std::string_view name = "projection", float zNear = 0.1f, float zFar = 100.0f) const;

        void key_input(Movement dir);

        void mouse_input(float xOffset, float yOffset, bool constrainPitch = true);

        void scroll_input(float yOffset);

    private:
        void update_vectors();
    };
}


#endif //EOS_ARCBALL_CAMERA_H
