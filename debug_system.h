#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace renderer {
    class Renderer;
    class Camera;  // Add Camera forward declaration
}

namespace debug {

    struct DebugLine {
        glm::vec3 start;
        glm::vec3 end;
        glm::vec3 color;
        float duration;
        float timeRemaining;
    };

    struct DebugText {
        std::string text;
        glm::vec2 position;
        glm::vec3 color;
        float scale;
        float duration;
        float timeRemaining;
    };

    class DebugViewer {
    public:
        DebugViewer();
        ~DebugViewer();

        bool initialize(GLFWwindow* window);
        void shutdown();
        void update(float deltaTime);
        void render(renderer::Renderer* renderer);

        // Set camera reference
        void setCamera(renderer::Camera* camera);

        // Debug drawing functions
        void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f), float duration = 0.0f);
        void drawBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& color = glm::vec3(1.0f), float duration = 0.0f);
        void drawSphere(const glm::vec3& center, float radius, const glm::vec3& color = glm::vec3(1.0f), float duration = 0.0f);
        void drawText(const std::string& text, const glm::vec2& position, const glm::vec3& color = glm::vec3(1.0f), float scale = 1.0f, float duration = 0.0f);
        void drawText3D(const std::string& text, const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f), float scale = 1.0f, float duration = 0.0f);

        // Performance metrics
        void setShowPerformanceMetrics(bool show);
        bool isShowingPerformanceMetrics() const;

    private:
        void updatePerformanceMetrics(float deltaTime);
        void renderPerformanceMetrics(renderer::Renderer* renderer);

        // Debug drawing data
        std::vector<DebugLine> m_lines;
        std::vector<DebugText> m_texts;

        // Performance metrics
        bool m_showPerformanceMetrics;
        float m_frameTime;
        float m_fps;
        float m_cpuTime;
        float m_smoothedFps;
        int m_frameCount;
        float m_timeSinceLastFpsUpdate;

        // Window
        GLFWwindow* m_window;
        int m_windowWidth;
        int m_windowHeight;

        // Camera reference
        renderer::Camera* m_camera;
    };

    class DebugSystem {
    public:
        DebugSystem();
        ~DebugSystem();

        bool initialize(GLFWwindow* window);
        void shutdown();
        void update(float deltaTime);
        void render(renderer::Renderer* renderer);

        // Set camera reference
        void setCamera(renderer::Camera* camera);

        // Debug viewer access
        DebugViewer* getViewer() const;

    private:
        DebugViewer* m_viewer;
    };

} // namespace debug

