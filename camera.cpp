#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

namespace renderer {

    Camera::Camera()
        : m_position(0.0f, 0.0f, 3.0f)
        , m_front(0.0f, 0.0f, -1.0f)
        , m_up(0.0f, 1.0f, 0.0f)
        , m_right(1.0f, 0.0f, 0.0f)
        , m_worldUp(0.0f, 1.0f, 0.0f)
        , m_yaw(-90.0f)
        , m_pitch(0.0f)
        , m_fov(45.0f)
        , m_aspectRatio(16.0f / 9.0f)
        , m_nearPlane(0.1f)
        , m_farPlane(100.0f)
    {
        updateCameraVectors();
    }

    Camera::~Camera() {
    }

    void Camera::setPosition(const glm::vec3& position) {
        m_position = position;
    }

    void Camera::setRotation(float yaw, float pitch) {
        std::cout << "Camera::setRotation called with Yaw: " << yaw << ", Pitch: " << pitch << std::endl;

        m_yaw = yaw;
        m_pitch = std::clamp(pitch, -89.0f, 89.0f);
        updateCameraVectors();

        std::cout << "Camera vectors after update - Front: ("
            << m_front.x << ", " << m_front.y << ", " << m_front.z
            << "), Up: (" << m_up.x << ", " << m_up.y << ", " << m_up.z
            << "), Right: (" << m_right.x << ", " << m_right.y << ", " << m_right.z << ")" << std::endl;
    }

    void Camera::move(const glm::vec3& offset) {
        m_position += offset;
    }

    void Camera::rotate(float yawOffset, float pitchOffset) {
        m_yaw += yawOffset;
        m_pitch += pitchOffset;

        // Constrain pitch
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

        updateCameraVectors();
    }

    glm::vec3 Camera::getPosition() const {
        return m_position;
    }

    glm::vec3 Camera::getFront() const {
        return m_front;
    }

    glm::vec3 Camera::getUp() const {
        return m_up;
    }

    glm::vec3 Camera::getRight() const {
        return m_right;
    }

    float Camera::getYaw() const {
        return m_yaw;
    }

    float Camera::getPitch() const {
        return m_pitch;
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    }

    void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        m_fov = fov;
        m_aspectRatio = aspectRatio;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
    }

    void Camera::updateCameraVectors() {
        std::cout << "Camera::updateCameraVectors called with Yaw: " << m_yaw << ", Pitch: " << m_pitch << std::endl;

        // Calculate the new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);

        // Recalculate the right and up vectors
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));

        std::cout << "Camera vectors calculated - Front: ("
            << m_front.x << ", " << m_front.y << ", " << m_front.z << ")" << std::endl;
    }

} // namespace renderer

