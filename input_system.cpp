#include "input_system.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <iomanip> // For std::setprecision

namespace input {

    // Static instance pointer for callbacks
    static InputSystem* s_instance = nullptr;

    InputSystem::InputSystem()
        : m_camera(nullptr)
        , m_cameraMovementSpeed(5.0f)  // Increased from 2.5f to 5.0f for faster movement
        , m_cameraRotationSpeed(0.1f)
        , m_cameraControlEnabled(true)
        , m_mousePosition(0.0f, 0.0f)
        , m_lastMousePosition(0.0f, 0.0f)
        , m_mouseDelta(0.0f, 0.0f)
        , m_mouseScrollDelta(0.0f)
        , m_firstMouse(true)
        , m_window(nullptr)
        , m_debugCounter(0)
    {
    }

    InputSystem::~InputSystem() {
        shutdown();
    }

    bool InputSystem::initialize(GLFWwindow* window, renderer::Camera* camera) {
        m_window = window;
        m_camera = camera;

        // Set static instance for callbacks
        s_instance = this;

        // Set GLFW callbacks
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetWindowFocusCallback(window, windowFocusCallback);

        // Capture mouse - FORCE DISABLED MODE
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Get initial mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        m_mousePosition = glm::vec2(xpos, ypos);
        m_lastMousePosition = m_mousePosition;

        std::cout << "Input system initialized. Mouse control enabled." << std::endl;
        std::cout << "DEBUG: Input system cursor mode: "
            << (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? "DISABLED" : "NORMAL") << std::endl;
        std::cout << "DEBUG: Initial mouse position: (" << xpos << ", " << ypos << ")" << std::endl;

        return true;
    }

    void InputSystem::shutdown() {
        if (m_window) {
            // Reset GLFW callbacks
            glfwSetKeyCallback(m_window, nullptr);
            glfwSetMouseButtonCallback(m_window, nullptr);
            glfwSetCursorPosCallback(m_window, nullptr);
            glfwSetScrollCallback(m_window, nullptr);
            glfwSetFramebufferSizeCallback(m_window, nullptr);
            glfwSetWindowFocusCallback(m_window, nullptr);

            // Release mouse
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            m_window = nullptr;
        }

        // Clear callbacks
        m_keyCallbacks.clear();
        m_mouseButtonCallbacks.clear();
        m_mouseMoveCallbacks.clear();
        m_mouseScrollCallbacks.clear();

        // Reset static instance
        if (s_instance == this) {
            s_instance = nullptr;
        }
    }

    void InputSystem::update(float deltaTime) {
        // Update key states
        for (auto& pair : m_keyStates) {
            if (pair.second == KeyState::PRESSED) {
                pair.second = KeyState::HELD;
            }
            else if (pair.second == KeyState::RELEASED_THIS_FRAME) {
                pair.second = KeyState::RELEASED;
            }
        }

        // DIRECT MOUSE HANDLING - Get current mouse position and calculate delta
        if (m_window && m_cameraControlEnabled) {
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);

            glm::vec2 currentPos(xpos, ypos);

            // Calculate delta
            if (m_firstMouse) {
                m_lastMousePosition = currentPos;
                m_firstMouse = false;
                m_mouseDelta = glm::vec2(0.0f, 0.0f);
            }
            else {
                m_mouseDelta = currentPos - m_lastMousePosition;
                m_lastMousePosition = currentPos;
            }

            // Apply camera rotation directly here
            if (m_camera && (m_mouseDelta.x != 0.0f || m_mouseDelta.y != 0.0f)) {
                float yaw = m_camera->getYaw() + m_mouseDelta.x * m_cameraRotationSpeed;
                float pitch = m_camera->getPitch() - m_mouseDelta.y * m_cameraRotationSpeed;

                // Clamp pitch to avoid flipping
                pitch = std::clamp(pitch, -89.0f, 89.0f);

                // Apply rotation to camera
                m_camera->setRotation(yaw, pitch);
            }

            // Handle keyboard movement directly here
            float speed = m_cameraMovementSpeed * deltaTime;
            glm::vec3 position = m_camera->getPosition();

            if (isKeyHeld(GLFW_KEY_W)) {
                position += m_camera->getFront() * speed;
            }
            if (isKeyHeld(GLFW_KEY_S)) {
                position -= m_camera->getFront() * speed;
            }
            if (isKeyHeld(GLFW_KEY_A)) {
                position -= m_camera->getRight() * speed;
            }
            if (isKeyHeld(GLFW_KEY_D)) {
                position += m_camera->getRight() * speed;
            }
            if (isKeyHeld(GLFW_KEY_SPACE)) {
                position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
            }
            if (isKeyHeld(GLFW_KEY_LEFT_SHIFT)) {
                position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
            }

            // Update camera position
            m_camera->setPosition(position);

            // Reset cursor to center of screen to prevent it from reaching the edge
            int width, height;
            glfwGetWindowSize(m_window, &width, &height);
            glfwSetCursorPos(m_window, width / 2, height / 2);
            m_lastMousePosition = glm::vec2(width / 2, height / 2);
        }
    }

    void InputSystem::resetMouseDelta() {
        m_mouseDelta = glm::vec2(0.0f, 0.0f);
        m_mouseScrollDelta = 0.0f;
    }

    bool InputSystem::isKeyPressed(int key) const {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second == KeyState::PRESSED;
    }

    bool InputSystem::isKeyReleased(int key) const {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second == KeyState::RELEASED_THIS_FRAME;
    }

    bool InputSystem::isKeyHeld(int key) const {
        auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && (it->second == KeyState::HELD || it->second == KeyState::PRESSED);
    }

    KeyState InputSystem::getKeyState(int key) const {
        auto it = m_keyStates.find(key);
        if (it != m_keyStates.end()) {
            return it->second;
        }
        return KeyState::RELEASED;
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const {
        int glfwButton = 0;
        switch (button) {
        case MouseButton::LEFT: glfwButton = GLFW_MOUSE_BUTTON_LEFT; break;
        case MouseButton::RIGHT: glfwButton = GLFW_MOUSE_BUTTON_RIGHT; break;
        case MouseButton::MIDDLE: glfwButton = GLFW_MOUSE_BUTTON_MIDDLE; break;
        }

        auto it = m_mouseButtonStates.find(glfwButton);
        return it != m_mouseButtonStates.end() && it->second;
    }

    bool InputSystem::isMouseButtonReleased(MouseButton button) const {
        return !isMouseButtonPressed(button);
    }

    bool InputSystem::isMouseButtonHeld(MouseButton button) const {
        return isMouseButtonPressed(button);
    }

    glm::vec2 InputSystem::getMousePosition() const {
        return m_mousePosition;
    }

    glm::vec2 InputSystem::getMouseDelta() const {
        return m_mouseDelta;
    }

    float InputSystem::getMouseScrollDelta() const {
        return m_mouseScrollDelta;
    }

    void InputSystem::setCameraMovementSpeed(float speed) {
        m_cameraMovementSpeed = speed;
    }

    void InputSystem::setCameraRotationSpeed(float speed) {
        m_cameraRotationSpeed = speed;
    }

    void InputSystem::enableCameraControl(bool enable) {
        m_cameraControlEnabled = enable;

        // Toggle cursor visibility
        if (m_window) {
            glfwSetInputMode(m_window, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            std::cout << "DEBUG: Camera control " << (enable ? "enabled" : "disabled")
                << ", cursor mode set to " << (enable ? "DISABLED" : "NORMAL") << std::endl;
        }

        // Reset first mouse flag
        m_firstMouse = true;
    }

    bool InputSystem::isCameraControlEnabled() const {
        return m_cameraControlEnabled;
    }

    void InputSystem::registerKeyCallback(const std::string& name, std::function<void(int, KeyState)> callback) {
        m_keyCallbacks[name] = callback;
    }

    void InputSystem::unregisterKeyCallback(const std::string& name) {
        m_keyCallbacks.erase(name);
    }

    void InputSystem::registerMouseButtonCallback(const std::string& name, std::function<void(MouseButton, bool)> callback) {
        m_mouseButtonCallbacks[name] = callback;
    }

    void InputSystem::unregisterMouseButtonCallback(const std::string& name) {
        m_mouseButtonCallbacks.erase(name);
    }

    void InputSystem::registerMouseMoveCallback(const std::string& name, std::function<void(double, double)> callback) {
        m_mouseMoveCallbacks[name] = callback;
    }

    void InputSystem::unregisterMouseMoveCallback(const std::string& name) {
        m_mouseMoveCallbacks.erase(name);
    }

    void InputSystem::registerMouseScrollCallback(const std::string& name, std::function<void(double)> callback) {
        m_mouseScrollCallbacks[name] = callback;
    }

    void InputSystem::unregisterMouseScrollCallback(const std::string& name) {
        m_mouseScrollCallbacks.erase(name);
    }

    // Static callback functions
    void InputSystem::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (!s_instance) return;

        // Update key state
        if (action == GLFW_PRESS) {
            s_instance->m_keyStates[key] = KeyState::PRESSED;
        }
        else if (action == GLFW_RELEASE) {
            s_instance->m_keyStates[key] = KeyState::RELEASED_THIS_FRAME;
        }

        // Call registered callbacks
        for (const auto& pair : s_instance->m_keyCallbacks) {
            pair.second(key, s_instance->m_keyStates[key]);
        }
    }

    void InputSystem::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (!s_instance) return;

        // Update mouse button state
        s_instance->m_mouseButtonStates[button] = (action == GLFW_PRESS);

        // Call registered callbacks
        MouseButton mb;
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: mb = MouseButton::LEFT; break;
        case GLFW_MOUSE_BUTTON_RIGHT: mb = MouseButton::RIGHT; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: mb = MouseButton::MIDDLE; break;
        default: return;
        }

        for (const auto& pair : s_instance->m_mouseButtonCallbacks) {
            pair.second(mb, action == GLFW_PRESS);
        }
    }

    // We're still keeping the callback for compatibility, but we're now using direct polling in update()
    void InputSystem::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        if (!s_instance) return;

        // Update mouse position
        s_instance->m_mousePosition = glm::vec2(xpos, ypos);

        // Call registered callbacks
        for (const auto& pair : s_instance->m_mouseMoveCallbacks) {
            pair.second(xpos, ypos);
        }
    }

    void InputSystem::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        if (!s_instance) return;

        // Update scroll delta
        s_instance->m_mouseScrollDelta = static_cast<float>(yoffset);

        // Call registered callbacks
        for (const auto& pair : s_instance->m_mouseScrollCallbacks) {
            pair.second(yoffset);
        }
    }

    void InputSystem::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        // Update viewport
        glViewport(0, 0, width, height);

        // Update camera aspect ratio if available
        if (s_instance && s_instance->m_camera) {
            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            s_instance->m_camera->setPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
        }
    }

    // Add window focus callback to handle tab-out
    void InputSystem::windowFocusCallback(GLFWwindow* window, int focused) {
        if (!s_instance) return;

        if (focused) {
            // Window gained focus
            s_instance->m_firstMouse = true;  // Reset mouse tracking to avoid jumps
        }
        else {
            // Window lost focus - clear key states to avoid stuck keys
            for (auto& pair : s_instance->m_keyStates) {
                if (pair.second == KeyState::HELD || pair.second == KeyState::PRESSED) {
                    pair.second = KeyState::RELEASED;
                }
            }

            // Clear mouse button states
            for (auto& pair : s_instance->m_mouseButtonStates) {
                pair.second = false;
            }
        }
    }

} // namespace input

