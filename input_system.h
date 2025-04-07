#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace renderer {
    class Camera;
}

namespace input {

    enum class KeyState {
        RELEASED,
        PRESSED,
        HELD,
        RELEASED_THIS_FRAME
    };

    enum class MouseButton {
        LEFT,
        RIGHT,
        MIDDLE
    };

    class InputSystem {
    public:
        InputSystem();
        ~InputSystem();

        bool initialize(GLFWwindow* window, renderer::Camera* camera);
        void shutdown();
        void update(float deltaTime);

        // Keyboard input
        bool isKeyPressed(int key) const;
        bool isKeyReleased(int key) const;
        bool isKeyHeld(int key) const;
        KeyState getKeyState(int key) const;

        // Mouse input
        bool isMouseButtonPressed(MouseButton button) const;
        bool isMouseButtonReleased(MouseButton button) const;
        bool isMouseButtonHeld(MouseButton button) const;
        glm::vec2 getMousePosition() const;
        glm::vec2 getMouseDelta() const;
        float getMouseScrollDelta() const;

        // Camera control
        void setCameraMovementSpeed(float speed);
        void setCameraRotationSpeed(float speed);
        void enableCameraControl(bool enable);
        bool isCameraControlEnabled() const;

        // Callbacks
        void registerKeyCallback(const std::string& name, std::function<void(int, KeyState)> callback);
        void unregisterKeyCallback(const std::string& name);
        void registerMouseButtonCallback(const std::string& name, std::function<void(MouseButton, bool)> callback);
        void unregisterMouseButtonCallback(const std::string& name);
        void registerMouseMoveCallback(const std::string& name, std::function<void(double, double)> callback);
        void unregisterMouseMoveCallback(const std::string& name);
        void registerMouseScrollCallback(const std::string& name, std::function<void(double)> callback);
        void unregisterMouseScrollCallback(const std::string& name);

        // Add this new method
        void resetMouseDelta();

    private:
        // GLFW callback wrappers
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void windowFocusCallback(GLFWwindow* window, int focused);

        // Input state
        std::unordered_map<int, KeyState> m_keyStates;
        std::unordered_map<int, bool> m_mouseButtonStates;
        glm::vec2 m_mousePosition;
        glm::vec2 m_lastMousePosition;
        glm::vec2 m_mouseDelta;
        float m_mouseScrollDelta;
        bool m_firstMouse;

        // Camera control
        renderer::Camera* m_camera;
        float m_cameraMovementSpeed;
        float m_cameraRotationSpeed;
        bool m_cameraControlEnabled;

        // Callbacks
        std::unordered_map<std::string, std::function<void(int, KeyState)>> m_keyCallbacks;
        std::unordered_map<std::string, std::function<void(MouseButton, bool)>> m_mouseButtonCallbacks;
        std::unordered_map<std::string, std::function<void(double, double)>> m_mouseMoveCallbacks;
        std::unordered_map<std::string, std::function<void(double)>> m_mouseScrollCallbacks;

        // GLFW window
        GLFWwindow* m_window;

        // Debug counter to limit log output
        int m_debugCounter;
    };

} // namespace input

