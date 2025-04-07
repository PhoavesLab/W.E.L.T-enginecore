#include "viewer.h"
#include "engine_core.h"
#include "renderer.h"
#include "camera.h"
#include "input_system.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <iomanip> // For std::setprecision

namespace game {

    Viewer::Viewer()
        : m_engineCore(nullptr)
        , m_camera(nullptr)
        , m_renderer(nullptr)
        , m_viewMode(ViewMode::FIRST_PERSON) // Changed default to FIRST_PERSON
        , m_cameraSpeed(5.0f)
        , m_cameraSensitivity(0.1f)
        , m_fpPosition(0.0f, 2.0f, 5.0f)
        , m_fpYaw(-90.0f)
        , m_fpPitch(0.0f)
        , m_tpTarget(0.0f, 0.0f, 0.0f)
        , m_tpDistance(10.0f)
        , m_tpYaw(-90.0f)
        , m_tpPitch(30.0f)
        , m_editorPosition(0.0f, 5.0f, 10.0f)
        , m_editorYaw(-90.0f)
        , m_editorPitch(-30.0f)
    {
    }

    Viewer::~Viewer() {
        shutdown();
    }

    bool Viewer::initialize(engine::EngineCore* engineCore) {
        m_engineCore = engineCore;

        if (!m_engineCore) {
            std::cerr << "Failed to initialize viewer: Engine core is null" << std::endl;
            return false;
        }

        m_camera = m_engineCore->getCamera();
        m_renderer = m_engineCore->getRenderer();

        if (!m_camera || !m_renderer) {
            std::cerr << "Failed to initialize viewer: Camera or renderer is null" << std::endl;
            return false;
        }

        // Set initial camera position based on view mode
        switch (m_viewMode) {
        case ViewMode::FIRST_PERSON:
            m_camera->setPosition(m_fpPosition);
            m_camera->setRotation(m_fpYaw, m_fpPitch);
            break;

        case ViewMode::THIRD_PERSON: {
            // Calculate camera position based on target, distance, and angles
            float horizontalDistance = m_tpDistance * cos(glm::radians(m_tpPitch));
            float verticalDistance = m_tpDistance * sin(glm::radians(m_tpPitch));

            float offsetX = horizontalDistance * cos(glm::radians(m_tpYaw));
            float offsetZ = horizontalDistance * sin(glm::radians(m_tpYaw));

            glm::vec3 cameraPos = m_tpTarget + glm::vec3(offsetX, verticalDistance, offsetZ);
            m_camera->setPosition(cameraPos);

            // Look at target
            m_camera->setRotation(m_tpYaw, -m_tpPitch);
            break;
        }

        case ViewMode::EDITOR:
        default:
            m_camera->setPosition(m_editorPosition);
            m_camera->setRotation(m_editorYaw, m_editorPitch);
            break;
        }

        std::cout << "Viewer initialized in " <<
            (m_viewMode == ViewMode::EDITOR ? "editor" :
                m_viewMode == ViewMode::FIRST_PERSON ? "first-person" : "third-person")
            << " mode" << std::endl;

        std::cout << "Initial camera position: (" << m_camera->getPosition().x << ", "
            << m_camera->getPosition().y << ", " << m_camera->getPosition().z << ")" << std::endl;
        std::cout << "Initial camera rotation - Yaw: " << m_camera->getYaw()
            << ", Pitch: " << m_camera->getPitch() << std::endl;

        return true;
    }

    void Viewer::shutdown() {
        m_camera = nullptr;
        m_renderer = nullptr;
        m_engineCore = nullptr;
    }

    // Simplify the update method - movement is now handled in InputSystem
    void Viewer::update(float deltaTime) {
        if (!m_camera || !m_engineCore || !m_engineCore->getInputSystem()) return;

        // Update view-specific properties
        switch (m_viewMode) {
        case ViewMode::FIRST_PERSON:
            // Update first-person view properties
            m_fpPosition = m_camera->getPosition();
            m_fpYaw = m_camera->getYaw();
            m_fpPitch = m_camera->getPitch();
            break;

        case ViewMode::THIRD_PERSON:
            // Update third-person view properties
            m_tpYaw = m_camera->getYaw();
            m_tpPitch = -m_camera->getPitch();
            break;

        case ViewMode::EDITOR:
            // Update editor view properties
            m_editorPosition = m_camera->getPosition();
            m_editorYaw = m_camera->getYaw();
            m_editorPitch = m_camera->getPitch();
            break;
        }
    }

    void Viewer::render() {
        if (!m_renderer) return;

        // Render UI elements
        renderUI();
    }

    void Viewer::handleKeyInput(int key, input::KeyState state) {
        if (state != input::KeyState::PRESSED) return;

        // Handle view mode switching
        switch (key) {
        case GLFW_KEY_1:
            setViewMode(ViewMode::EDITOR);
            break;

        case GLFW_KEY_2:
            setViewMode(ViewMode::FIRST_PERSON);
            break;

        case GLFW_KEY_3:
            setViewMode(ViewMode::THIRD_PERSON);
            break;
        }
    }

    void Viewer::handleMouseInput(input::MouseButton button, bool pressed) {
        // Handle mouse input based on view mode
        if (m_viewMode == ViewMode::THIRD_PERSON && button == input::MouseButton::RIGHT) {
            std::cout << "Third-person right mouse button "
                << (pressed ? "pressed" : "released") << std::endl;
        }
    }

    void Viewer::setViewMode(ViewMode mode) {
        if (m_viewMode == mode) return;

        // Save current camera settings for the old mode
        switch (m_viewMode) {
        case ViewMode::FIRST_PERSON:
            m_fpPosition = m_camera->getPosition();
            m_fpYaw = m_camera->getYaw();
            m_fpPitch = m_camera->getPitch();
            break;

        case ViewMode::THIRD_PERSON:
            // Calculate target position based on current camera position and direction
            m_tpYaw = m_camera->getYaw();
            m_tpPitch = -m_camera->getPitch();
            break;

        case ViewMode::EDITOR:
            m_editorPosition = m_camera->getPosition();
            m_editorYaw = m_camera->getYaw();
            m_editorPitch = m_camera->getPitch();
            break;
        }

        // Apply new camera settings
        switch (mode) {
        case ViewMode::FIRST_PERSON:
            m_camera->setPosition(m_fpPosition);
            m_camera->setRotation(m_fpYaw, m_fpPitch);
            break;

        case ViewMode::THIRD_PERSON: {
            // Calculate camera position based on target, distance, and angles
            float horizontalDistance = m_tpDistance * cos(glm::radians(m_tpPitch));
            float verticalDistance = m_tpDistance * sin(glm::radians(m_tpPitch));

            float offsetX = horizontalDistance * cos(glm::radians(m_tpYaw));
            float offsetZ = horizontalDistance * sin(glm::radians(m_tpYaw));

            glm::vec3 cameraPos = m_tpTarget + glm::vec3(offsetX, verticalDistance, offsetZ);
            m_camera->setPosition(cameraPos);

            // Look at target
            m_camera->setRotation(m_tpYaw, -m_tpPitch);
            break;
        }

        case ViewMode::EDITOR:
            m_camera->setPosition(m_editorPosition);
            m_camera->setRotation(m_editorYaw, m_editorPitch);
            break;
        }

        m_viewMode = mode;

        std::cout << "Switched to " <<
            (m_viewMode == ViewMode::EDITOR ? "editor" :
                m_viewMode == ViewMode::FIRST_PERSON ? "first-person" : "third-person")
            << " view mode" << std::endl;
    }

    ViewMode Viewer::getViewMode() const {
        return m_viewMode;
    }

    void Viewer::setCameraPosition(const glm::vec3& position) {
        if (!m_camera) return;

        m_camera->setPosition(position);

        // Update mode-specific position
        switch (m_viewMode) {
        case ViewMode::FIRST_PERSON:
            m_fpPosition = position;
            break;

        case ViewMode::THIRD_PERSON:
            // Adjust target based on new position
            break;

        case ViewMode::EDITOR:
            m_editorPosition = position;
            break;
        }
    }

    void Viewer::setCameraRotation(float yaw, float pitch) {
        if (!m_camera) return;

        m_camera->setRotation(yaw, pitch);

        // Update mode-specific rotation
        switch (m_viewMode) {
        case ViewMode::FIRST_PERSON:
            m_fpYaw = yaw;
            m_fpPitch = pitch;
            break;

        case ViewMode::THIRD_PERSON:
            m_tpYaw = yaw;
            m_tpPitch = -pitch;
            break;

        case ViewMode::EDITOR:
            m_editorYaw = yaw;
            m_editorPitch = pitch;
            break;
        }
    }

    void Viewer::setCameraTarget(const glm::vec3& target) {
        m_tpTarget = target;

        if (m_viewMode == ViewMode::THIRD_PERSON && m_camera) {
            // Recalculate camera position
            float horizontalDistance = m_tpDistance * cos(glm::radians(m_tpPitch));
            float verticalDistance = m_tpDistance * sin(glm::radians(m_tpPitch));

            float offsetX = horizontalDistance * cos(glm::radians(m_tpYaw));
            float offsetZ = horizontalDistance * sin(glm::radians(m_tpYaw));

            glm::vec3 cameraPos = m_tpTarget + glm::vec3(offsetX, verticalDistance, offsetZ);
            m_camera->setPosition(cameraPos);

            // Look at target
            m_camera->setRotation(m_tpYaw, -m_tpPitch);
        }
    }

    void Viewer::renderUI() {
        if (!m_renderer) return;

        // Render crosshair
        renderCrosshair();

        // Render HUD
        renderHUD();
    }

    void Viewer::renderCrosshair() {
        if (!m_renderer) return;

        // Only show crosshair in first-person mode
        if (m_viewMode != ViewMode::FIRST_PERSON) return;

        int windowWidth = m_renderer->getWindowWidth();
        int windowHeight = m_renderer->getWindowHeight();

        // Draw crosshair
        m_renderer->beginUI();

        float crosshairSize = 10.0f;
        float centerX = windowWidth / 2.0f;
        float centerY = windowHeight / 2.0f;

        // Horizontal line
        m_renderer->drawLine2D(
            centerX - crosshairSize, centerY,
            centerX + crosshairSize, centerY,
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        // Vertical line
        m_renderer->drawLine2D(
            centerX, centerY - crosshairSize,
            centerX, centerY + crosshairSize,
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        m_renderer->endUI();
    }

    void Viewer::renderHUD() {
        if (!m_renderer) return;

        int windowWidth = m_renderer->getWindowWidth();
        int windowHeight = m_renderer->getWindowHeight();

        m_renderer->beginUI();

        // Draw mode indicator
        std::string modeText;
        switch (m_viewMode) {
        case ViewMode::EDITOR:
            modeText = "Mode: Editor (1)";
            break;

        case ViewMode::FIRST_PERSON:
            modeText = "Mode: First Person (2)";
            break;

        case ViewMode::THIRD_PERSON:
            modeText = "Mode: Third Person (3)";
            break;
        }

        // Draw background
        m_renderer->drawRect(10, windowHeight - 30, modeText.length() * 8 + 10, 20, glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));
        // Draw text
        m_renderer->drawText(modeText, 15, windowHeight - 25, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        // Draw controls help
        std::string controlsText = "Controls: WASD - Move | Mouse - Look | 1/2/3 - Change View | F - Toggle Wireframe | G - Toggle Debug | ESC - Exit";

        // Draw background
        m_renderer->drawRect(10, windowHeight - 55, controlsText.length() * 6 + 10, 20, glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));

        // Draw text
        m_renderer->drawText(controlsText, 15, windowHeight - 50, 0.8f, glm::vec3(0.8f, 0.8f, 0.8f));

        m_renderer->endUI();
    }
} // namespace game

