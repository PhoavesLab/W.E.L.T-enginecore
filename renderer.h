#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace renderer {

    class Shader;
    class Mesh;
    class Camera;

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        bool initialize(int windowWidth, int windowHeight, const std::string& title);
        void shutdown();

        void beginFrame();
        void endFrame();

        void setWireframeMode(bool enabled);
        bool isWireframeMode() const;

        void drawMesh(const Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1.0f));
        void drawLines(const std::vector<float>& vertices, const glm::vec3& color = glm::vec3(1.0f));

        // 2D rendering for UI
        void beginUI();
        void endUI();
        void drawText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
        void drawRect(float x, float y, float width, float height, const glm::vec4& color);
        void drawLine2D(float x1, float y1, float x2, float y2, const glm::vec3& color, float thickness = 1.0f);

        // Window management
        GLFWwindow* getWindow() const;
        int getWindowWidth() const;
        int getWindowHeight() const;
        void setViewport(int width, int height);

        // Shader management
        Shader* getShader(const std::string& name);
        void addShader(const std::string& name, Shader* shader);

        // Add setter for camera
        void setCamera(renderer::Camera* camera);

    private:
        bool initializeOpenGL();
        bool createDefaultShaders();

        GLFWwindow* m_window;
        int m_windowWidth;
        int m_windowHeight;
        bool m_wireframeMode;

        // Shaders
        std::unordered_map<std::string, Shader*> m_shaders;
        Shader* m_activeShader;

        // OpenGL objects for UI rendering
        unsigned int m_uiVAO;
        unsigned int m_uiVBO;

        // Camera reference
        renderer::Camera* m_camera;
    };

} // namespace renderer

