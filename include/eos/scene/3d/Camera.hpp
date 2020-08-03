//
// Created by jakob on 19.07.20.
//

#ifndef EOS_CAMERA_HPP
#define EOS_CAMERA_HPP

#include <glm/glm.hpp>
#include <string>
#include "eos/Constants.hpp"

namespace eos {

    class Shader;

    class Camera {
    public:
        glm::vec3 pos_;
        glm::vec3 target_{0.0f, 0.0f, -1.0f};

        constexpr static const glm::vec3 up_{0.0f, 1.0f, 0.0f};

        float yaw_{-90.0f}; // Rotation around y-axis (left/right)
        float pitch_{0.0f}; // Rotation around x-axis (up/down)
        float fov_{glm::radians(90.0f)};

        explicit Camera(const glm::vec3& pos = {0.0f, 0.0f, 0.0f});

        Camera(const glm::vec3& pos, float yaw, float pitch);

        Camera& move(eos::Direction dir);

        Camera& move(const glm::vec3& dir);

        // longitudinal
        Camera& move_long(float dist);
        Camera& move_forward(float dist);
        Camera& move_backward(float dist);

        // lateral
        Camera& move_lat(float dist);
        Camera& move_right(float dist);
        Camera& move_left(float dist);

        // vertical
        Camera& move_vert(float dist);
        Camera& move_up(float dist);
        Camera& move_down(float dist);
        
        Camera& teleport(const glm::vec3& pos);

        Camera& rotate(float deltaX, float deltaY, bool constrainPitch = true);

        [[nodiscard]] glm::mat4 get_projection_matrix(float zNear = 0.1f, float zFar = 100.0f) const;

        [[nodiscard]] glm::mat4 get_view_matrix() const;

        void apply_view_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders, const std::string& name = "view") const;

        void apply_projection_matrix(const std::initializer_list<std::shared_ptr<eos::Shader>>& shaders, const std::string& name = "projection", float zNear = 0.1f, float zFar = 100.0f) const;

    private:
        void recalculate_vectors();
    };
}

#endif //EOS_CAMERA_HPP
