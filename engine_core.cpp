#include "engine_core.h"
#include "renderer.h"
#include "camera.h"
#include "input_system.h"
#include "voxel_system.h"
#include "debug_system.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

namespace engine {

    EngineCore::EngineCore()
        : m_isRunning(false)
        , m_lastFrameTime(0.0f)
        , m_deltaTime(0.0f)
    {
    }

    EngineCore::~EngineCore() {
        shutdown();
    }

    bool EngineCore::initialize(int windowWidth, int windowHeight, const std::string& title) {
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        // Set error callback
        glfwSetErrorCallback([](int error, const char* description) {
            std::cerr << "GLFW Error " << error << ": " << description << std::endl;
            });

        // Create renderer first (sets up OpenGL context)
        m_renderer = std::make_unique<renderer::Renderer>();
        if (!m_renderer->initialize(windowWidth, windowHeight, title)) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return false;
        }

        // Create camera with proper aspect ratio
        m_camera = std::make_unique<renderer::Camera>();
        m_camera->setPerspective(45.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);
        m_camera->setPosition(glm::vec3(0.0f, 2.0f, 5.0f));

        // Set camera in renderer
        m_renderer->setCamera(m_camera.get());

        // Create input system
        m_inputSystem = std::make_unique<input::InputSystem>();
        if (!m_inputSystem->initialize(m_renderer->getWindow(), m_camera.get())) {
            std::cerr << "Failed to initialize input system" << std::endl;
            return false;
        }

        // Create voxel system
        m_voxelSystem = std::make_unique<voxel::VoxelSystem>();
        if (!m_voxelSystem->initialize()) {
            std::cerr << "Failed to initialize voxel system" << std::endl;
            return false;
        }

        // Create debug system
        m_debugSystem = std::make_unique<debug::DebugSystem>();
        if (!m_debugSystem->initialize(m_renderer->getWindow())) {
            std::cerr << "Failed to initialize debug system" << std::endl;
            return false;
        }

        // Set camera in debug system
        m_debugSystem->setCamera(m_camera.get());

        m_isRunning = true;
        m_lastFrameTime = static_cast<float>(glfwGetTime());

        return true;
    }

    void EngineCore::shutdown() {
        // Shutdown in reverse order of initialization
        if (m_debugSystem) {
            m_debugSystem->shutdown();
        }

        if (m_voxelSystem) {
            m_voxelSystem->shutdown();
        }

        if (m_inputSystem) {
            m_inputSystem->shutdown();
        }

        if (m_renderer) {
            m_renderer->shutdown();
        }

        glfwTerminate();
        m_isRunning = false;
    }

    void EngineCore::run() {
        while (m_isRunning && !glfwWindowShouldClose(m_renderer->getWindow())) {
            // Calculate delta time
            float currentTime = static_cast<float>(glfwGetTime());
            m_deltaTime = currentTime - m_lastFrameTime;
            m_lastFrameTime = currentTime;

            // Poll for events first - this is critical for proper input handling
            glfwPollEvents();

            // Process input first
            m_inputSystem->update(m_deltaTime);

            // Then update game systems
            update(m_deltaTime);

            // Render frame
            render();
        }
    }

    bool EngineCore::isRunning() const {
        return m_isRunning;
    }

    void EngineCore::stop() {
        m_isRunning = false;
    }

    void EngineCore::update(float deltaTime) {
        // Update all systems except input (already updated)
        m_voxelSystem->update(deltaTime);
        m_debugSystem->update(deltaTime);
    }

    void EngineCore::render() {
        // Clear the screen
        m_renderer->beginFrame();

        // Render voxel world
        m_voxelSystem->render(m_renderer.get(), m_camera.get());

        // Render debug information
        m_debugSystem->render(m_renderer.get());

        // Finish rendering
        m_renderer->endFrame();
    }

    renderer::Renderer* EngineCore::getRenderer() const {
        return m_renderer.get();
    }

    renderer::Camera* EngineCore::getCamera() const {
        return m_camera.get();
    }

    input::InputSystem* EngineCore::getInputSystem() const {
        return m_inputSystem.get();
    }

    voxel::VoxelSystem* EngineCore::getVoxelSystem() const {
        return m_voxelSystem.get();
    }

    debug::DebugSystem* EngineCore::getDebugSystem() const {
        return m_debugSystem.get();
    }

} // namespace engine

