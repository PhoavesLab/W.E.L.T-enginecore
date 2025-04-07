#pragma once

#include <glm/glm.hpp>
#include "input_system.h" // Add this include

namespace engine {
    class EngineCore;
}

namespace renderer {
    class Renderer;
    class Camera;
}

namespace input {
    enum class KeyState;
    enum class MouseButton;
}

namespace game {

    enum class ViewMode {
        EDITOR,
        FIRST_PERSON,
        THIRD_PERSON
    };

    class Viewer {
    public:
        Viewer();
        ~Viewer();

        bool initialize(engine::EngineCore* engineCore);
        void shutdown();
        void update(float deltaTime);
        void render();

        // Input handling
        void handleKeyInput(int key, input::KeyState state);
        void handleMouseInput(input::MouseButton button, bool pressed);

        // View mode
        void setViewMode(ViewMode mode);
        ViewMode getViewMode() const;

        // Camera control
        void setCameraPosition(const glm::vec3& position);
        void setCameraRotation(float yaw, float pitch);
        void setCameraTarget(const glm::vec3& target);

        // UI
        void renderUI();
        void renderCrosshair();
        void renderHUD();

    private:
        engine::EngineCore* m_engineCore;
        renderer::Camera* m_camera;
        renderer::Renderer* m_renderer;

        ViewMode m_viewMode;

        // Camera settings
        float m_cameraSpeed;
        float m_cameraSensitivity;

        // First-person view
        glm::vec3 m_fpPosition;
        float m_fpYaw;
        float m_fpPitch;

        // Third-person view
        glm::vec3 m_tpTarget;
        float m_tpDistance;
        float m_tpYaw;
        float m_tpPitch;

        // Editor view
        glm::vec3 m_editorPosition;
        float m_editorYaw;
        float m_editorPitch;

        // Helper methods for different view modes
        void updateFirstPersonView(float deltaTime);
        void updateThirdPersonView(float deltaTime);
        void updateEditorView(float deltaTime);
    };

} // namespace game

