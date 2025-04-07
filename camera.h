#pragma once

#include <glm/glm.hpp>

namespace renderer {

    class Camera {
    public:
        Camera();
        ~Camera();

        // Camera control
        void setPosition(const glm::vec3& position);
        void setRotation(float yaw, float pitch);
        void move(const glm::vec3& offset);
        void rotate(float yawOffset, float pitchOffset);

        // Camera properties
        glm::vec3 getPosition() const;
        glm::vec3 getFront() const;
        glm::vec3 getUp() const;
        glm::vec3 getRight() const;
        float getYaw() const;
        float getPitch() const;

        // View and projection matrices
        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;
        void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

    private:
        void updateCameraVectors();

        // Camera position and orientation
        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_worldUp;

        // Euler angles
        float m_yaw;
        float m_pitch;

        // Camera options
        float m_fov;
        float m_aspectRatio;
        float m_nearPlane;
        float m_farPlane;
    };

} // namespace renderer

