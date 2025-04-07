#pragma once

#include <memory>
#include <string>

// Forward declarations
namespace renderer {
    class Renderer;
    class Camera;
}

namespace input {
    class InputSystem;
}

namespace voxel {
    class VoxelSystem;
}

namespace debug {
    class DebugSystem;
}

namespace engine {

    class EngineCore {
    public:
        EngineCore();
        ~EngineCore();

        bool initialize(int windowWidth, int windowHeight, const std::string& title);
        void shutdown();

        void run();
        bool isRunning() const;
        void stop();

        // Core system getters
        renderer::Renderer* getRenderer() const;
        renderer::Camera* getCamera() const;
        input::InputSystem* getInputSystem() const;
        voxel::VoxelSystem* getVoxelSystem() const;
        debug::DebugSystem* getDebugSystem() const;

    private:
        void update(float deltaTime);
        void render();

        // Core systems
        std::unique_ptr<renderer::Renderer> m_renderer;
        std::unique_ptr<renderer::Camera> m_camera;
        std::unique_ptr<input::InputSystem> m_inputSystem;
        std::unique_ptr<voxel::VoxelSystem> m_voxelSystem;
        std::unique_ptr<debug::DebugSystem> m_debugSystem;

        // Engine state
        bool m_isRunning;
        float m_lastFrameTime;
        float m_deltaTime;
    };

} // namespace engine

