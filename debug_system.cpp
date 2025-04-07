#include "debug_system.h"
#include "renderer.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace debug {

    // DebugViewer implementation
    DebugViewer::DebugViewer()
        : m_showPerformanceMetrics(true)
        , m_frameTime(0.0f)
        , m_fps(0.0f)
        , m_cpuTime(0.0f)
        , m_smoothedFps(0.0f)
        , m_frameCount(0)
        , m_timeSinceLastFpsUpdate(0.0f)
        , m_window(nullptr)
        , m_windowWidth(800)
        , m_windowHeight(600)
        , m_camera(nullptr)
    {
    }

    DebugViewer::~DebugViewer() {
        shutdown();
    }

    bool DebugViewer::initialize(GLFWwindow* window) {
        m_window = window;

        // Get window size
        glfwGetWindowSize(window, &m_windowWidth, &m_windowHeight);

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

        // Setup ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        return true;
    }

    void DebugViewer::shutdown() {
        m_lines.clear();
        m_texts.clear();

        // Shutdown ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void DebugViewer::setCamera(renderer::Camera* camera) {
        m_camera = camera;
    }

    void DebugViewer::update(float deltaTime) {
        // Update performance metrics
        updatePerformanceMetrics(deltaTime);

        // Update debug lines
        for (auto it = m_lines.begin(); it != m_lines.end();) {
            if (it->duration > 0.0f) {
                it->timeRemaining -= deltaTime;
                if (it->timeRemaining <= 0.0f) {
                    it = m_lines.erase(it);
                    continue;
                }
            }
            ++it;
        }

        // Update debug texts
        for (auto it = m_texts.begin(); it != m_texts.end();) {
            if (it->duration > 0.0f) {
                it->timeRemaining -= deltaTime;
                if (it->timeRemaining <= 0.0f) {
                    it = m_texts.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    void DebugViewer::render(renderer::Renderer* renderer) {
        if (!renderer) return;

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render debug lines
        for (const auto& line : m_lines) {
            std::vector<float> vertices = {
                line.start.x, line.start.y, line.start.z,
                line.end.x, line.end.y, line.end.z
            };

            renderer->drawLines(vertices, line.color);
        }

        // Render performance metrics with ImGui
        if (m_showPerformanceMetrics) {
            renderPerformanceMetrics(renderer);
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void DebugViewer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, float duration) {
        DebugLine line;
        line.start = start;
        line.end = end;
        line.color = color;
        line.duration = duration;
        line.timeRemaining = duration;

        m_lines.push_back(line);
    }

    void DebugViewer::drawBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& color, float duration) {
        // Draw 12 lines to form a box
        glm::vec3 corners[8] = {
            glm::vec3(min.x, min.y, min.z),
            glm::vec3(max.x, min.y, min.z),
            glm::vec3(max.x, max.y, min.z),
            glm::vec3(min.x, max.y, min.z),
            glm::vec3(min.x, min.y, max.z),
            glm::vec3(max.x, min.y, max.z),
            glm::vec3(max.x, max.y, max.z),
            glm::vec3(min.x, max.y, max.z)
        };

        // Bottom face
        drawLine(corners[0], corners[1], color, duration);
        drawLine(corners[1], corners[2], color, duration);
        drawLine(corners[2], corners[3], color, duration);
        drawLine(corners[3], corners[0], color, duration);

        // Top face
        drawLine(corners[4], corners[5], color, duration);
        drawLine(corners[5], corners[6], color, duration);
        drawLine(corners[6], corners[7], color, duration);
        drawLine(corners[7], corners[4], color, duration);

        // Connecting edges
        drawLine(corners[0], corners[4], color, duration);
        drawLine(corners[1], corners[5], color, duration);
        drawLine(corners[2], corners[6], color, duration);
        drawLine(corners[3], corners[7], color, duration);
    }

    void DebugViewer::drawSphere(const glm::vec3& center, float radius, const glm::vec3& color, float duration) {
        // Draw a wireframe sphere using lines
        const int segments = 12;

        // Draw three circles in each plane
        for (int i = 0; i < segments; i++) {
            float angle1 = (float)i / segments * 2.0f * 3.14159f;
            float angle2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

            // XY plane
            drawLine(
                center + glm::vec3(radius * cos(angle1), radius * sin(angle1), 0.0f),
                center + glm::vec3(radius * cos(angle2), radius * sin(angle2), 0.0f),
                color, duration
            );

            // XZ plane
            drawLine(
                center + glm::vec3(radius * cos(angle1), 0.0f, radius * sin(angle1)),
                center + glm::vec3(radius * cos(angle2), 0.0f, radius * sin(angle2)),
                color, duration
            );

            // YZ plane
            drawLine(
                center + glm::vec3(0.0f, radius * cos(angle1), radius * sin(angle1)),
                center + glm::vec3(0.0f, radius * cos(angle2), radius * sin(angle2)),
                color, duration
            );
        }
    }

    void DebugViewer::drawText(const std::string& text, const glm::vec2& position, const glm::vec3& color, float scale, float duration) {
        DebugText debugText;
        debugText.text = text;
        debugText.position = position;
        debugText.color = color;
        debugText.scale = scale;
        debugText.duration = duration;
        debugText.timeRemaining = duration;

        m_texts.push_back(debugText);
    }

    void DebugViewer::drawText3D(const std::string& text, const glm::vec3& position, const glm::vec3& color, float scale, float duration) {
        // This would require projecting the 3D position to 2D screen space
        // For simplicity, we'll just use the 2D version for now
        drawText(text, glm::vec2(position.x, position.y), color, scale, duration);
    }

    void DebugViewer::setShowPerformanceMetrics(bool show) {
        m_showPerformanceMetrics = show;
    }

    bool DebugViewer::isShowingPerformanceMetrics() const {
        return m_showPerformanceMetrics;
    }

    void DebugViewer::updatePerformanceMetrics(float deltaTime) {
        // Update frame count
        m_frameCount++;
        m_timeSinceLastFpsUpdate += deltaTime;

        // Update FPS every half second
        if (m_timeSinceLastFpsUpdate >= 0.5f) {
            m_fps = m_frameCount / m_timeSinceLastFpsUpdate;
            m_frameTime = 1000.0f / m_fps; // ms per frame

            // Smooth FPS for display
            if (m_smoothedFps == 0.0f) {
                m_smoothedFps = m_fps;
            }
            else {
                m_smoothedFps = 0.95f * m_smoothedFps + 0.05f * m_fps;
            }

            m_frameCount = 0;
            m_timeSinceLastFpsUpdate = 0.0f;
        }

        // Measure CPU time for this frame
        m_cpuTime = deltaTime * 1000.0f; // Convert to ms
    }

    void DebugViewer::renderPerformanceMetrics(renderer::Renderer* renderer) {
        // Create ImGui window for performance metrics
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Performance Metrics", &m_showPerformanceMetrics);

        ImGui::Text("FPS: %.1f", m_smoothedFps);
        ImGui::Text("Frame Time: %.2f ms", m_frameTime);
        ImGui::Text("CPU Time: %.2f ms", m_cpuTime);

        // Add a graph for FPS history
        static float fpsValues[100] = {};
        static int fpsOffset = 0;

        fpsValues[fpsOffset] = m_smoothedFps;
        fpsOffset = (fpsOffset + 1) % IM_ARRAYSIZE(fpsValues);

        float fpsAverage = 0.0f;
        for (int n = 0; n < IM_ARRAYSIZE(fpsValues); n++)
            fpsAverage += fpsValues[n];
        fpsAverage /= (float)IM_ARRAYSIZE(fpsValues);

        char overlay[32];
        sprintf_s(overlay, "Avg %.1f FPS", fpsAverage);
        ImGui::PlotLines("FPS", fpsValues, IM_ARRAYSIZE(fpsValues), fpsOffset, overlay, 0.0f, 200.0f, ImVec2(0, 80));

        // Add camera position info if available
        if (m_camera) {
            glm::vec3 pos = m_camera->getPosition();
            ImGui::Separator();
            ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            ImGui::Text("Camera Rotation - Yaw: %.2f, Pitch: %.2f",
                m_camera->getYaw(),
                m_camera->getPitch());
        }

        ImGui::End();
    }

    // DebugSystem implementation
    DebugSystem::DebugSystem()
        : m_viewer(nullptr)
    {
    }

    DebugSystem::~DebugSystem() {
        shutdown();
    }

    bool DebugSystem::initialize(GLFWwindow* window) {
        m_viewer = new DebugViewer();
        return m_viewer->initialize(window);
    }

    void DebugSystem::setCamera(renderer::Camera* camera) {
        if (m_viewer) {
            m_viewer->setCamera(camera);
        }
    }

    void DebugSystem::shutdown() {
        if (m_viewer) {
            m_viewer->shutdown();
            delete m_viewer;
            m_viewer = nullptr;
        }
    }

    void DebugSystem::update(float deltaTime) {
        if (m_viewer) {
            m_viewer->update(deltaTime);
        }
    }

    void DebugSystem::render(renderer::Renderer* renderer) {
        if (m_viewer) {
            m_viewer->render(renderer);
        }
    }

    DebugViewer* DebugSystem::getViewer() const {
        return m_viewer;
    }

} // namespace debug

