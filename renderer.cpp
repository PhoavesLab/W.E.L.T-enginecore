#include "renderer.h"
#include "shader.h"
#include "mesh.h"
#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace renderer {

    Renderer::Renderer()
        : m_window(nullptr)
        , m_windowWidth(800)
        , m_windowHeight(600)
        , m_wireframeMode(false)
        , m_activeShader(nullptr)
        , m_uiVAO(0)
        , m_uiVBO(0)
        , m_camera(nullptr)
    {
    }

    Renderer::~Renderer() {
        shutdown();
    }

    bool Renderer::initialize(int windowWidth, int windowHeight, const std::string& title) {
        m_windowWidth = windowWidth;
        m_windowHeight = windowHeight;

        // Initialize GLFW window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create window
        m_window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
        if (!m_window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return false;
        }

        glfwMakeContextCurrent(m_window);

        // Initialize OpenGL
        if (!initializeOpenGL()) {
            return false;
        }

        // Create default shaders
        if (!createDefaultShaders()) {
            return false;
        }

        // Set up UI rendering
        glGenVertexArrays(1, &m_uiVAO);
        glGenBuffers(1, &m_uiVBO);

        glBindVertexArray(m_uiVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Print OpenGL version info
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);
        std::cout << "Renderer: " << renderer << std::endl;
        std::cout << "OpenGL version: " << version << std::endl;

        return true;
    }

    void Renderer::shutdown() {
        // Clean up shaders
        for (auto& pair : m_shaders) {
            delete pair.second;
        }
        m_shaders.clear();

        // Clean up UI rendering
        if (m_uiVAO) {
            glDeleteVertexArrays(1, &m_uiVAO);
            m_uiVAO = 0;
        }

        if (m_uiVBO) {
            glDeleteBuffers(1, &m_uiVBO);
            m_uiVBO = 0;
        }

        // Destroy window
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    bool Renderer::initializeOpenGL() {
        // Load OpenGL functions
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        // Set viewport
        glViewport(0, 0, m_windowWidth, m_windowHeight);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Enable blending for transparent objects
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
    }

    bool Renderer::createDefaultShaders() {
        // Basic shader for 3D rendering
        const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 Normal;
        out vec3 FragPos;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

        const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 Normal;
        in vec3 FragPos;
        
        uniform vec3 lightPos;
        uniform vec3 viewPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        
        void main() {
            // Ambient
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;
            
            vec3 result = (ambient + diffuse + specular) * objectColor;
            FragColor = vec4(result, 1.0);
        }
    )";

        // Create basic shader
        Shader* basicShader = new Shader();
        if (!basicShader->compile(vertexShaderSource, fragmentShaderSource)) {
            delete basicShader;
            return false;
        }

        // Add to shader map
        m_shaders["basic"] = basicShader;

        // Line shader for grid and debug lines
        const char* lineVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * vec4(aPos, 1.0);
        }
    )";

        const char* lineFragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        uniform vec3 lineColor;
        
        void main() {
            FragColor = vec4(lineColor, 1.0);
        }
    )";

        // Create line shader
        Shader* lineShader = new Shader();
        if (!lineShader->compile(lineVertexShaderSource, lineFragmentShaderSource)) {
            delete lineShader;
            return false;
        }

        // Add to shader map
        m_shaders["line"] = lineShader;

        // UI shader for text and 2D elements
        const char* uiVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

        const char* uiFragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform sampler2D textTexture;
        uniform vec4 color;
        uniform bool hasTexture;
        
        void main() {
            if (hasTexture) {
                vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textTexture, TexCoord).r);
                FragColor = color * sampled;
            } else {
                FragColor = color;
            }
        }
    )";

        // Create UI shader
        Shader* uiShader = new Shader();
        if (!uiShader->compile(uiVertexShaderSource, uiFragmentShaderSource)) {
            delete uiShader;
            return false;
        }

        // Add to shader map
        m_shaders["ui"] = uiShader;

        return true;
    }

    void Renderer::beginFrame() {
        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::endFrame() {
        // Swap buffers
        glfwSwapBuffers(m_window);
    }

    void Renderer::setWireframeMode(bool enabled) {
        m_wireframeMode = enabled;
        if (enabled) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    bool Renderer::isWireframeMode() const {
        return m_wireframeMode;
    }

    void Renderer::setCamera(renderer::Camera* camera) {
        m_camera = camera;
    }

    // Fix the drawMesh method to use m_camera if available
    void Renderer::drawMesh(const Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color) {
        if (!mesh) return;

        // Use basic shader
        Shader* shader = getShader("basic");
        if (!shader) return;

        shader->use();

        // Set uniforms
        shader->setMat4("model", modelMatrix);

        if (m_camera) {
            shader->setMat4("view", m_camera->getViewMatrix());
            shader->setMat4("projection", m_camera->getProjectionMatrix());
            shader->setVec3("viewPos", m_camera->getPosition());
        }
        else {
            shader->setMat4("view", glm::mat4(1.0f));
            shader->setMat4("projection", glm::mat4(1.0f));
            shader->setVec3("viewPos", glm::vec3(0.0f));
        }

        shader->setVec3("objectColor", color);
        shader->setVec3("lightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Draw mesh
        mesh->draw();
    }

    // Fix the drawLines method to use m_camera if available
    void Renderer::drawLines(const std::vector<float>& vertices, const glm::vec3& color) {
        if (vertices.empty()) return;

        // Use line shader
        Shader* shader = getShader("line");
        if (!shader) return;

        shader->use();

        // Set uniforms
        if (m_camera) {
            shader->setMat4("view", m_camera->getViewMatrix());
            shader->setMat4("projection", m_camera->getProjectionMatrix());
        }
        else {
            shader->setMat4("view", glm::mat4(1.0f));
            shader->setMat4("projection", glm::mat4(1.0f));
        }

        shader->setVec3("lineColor", color);

        // Create temporary VAO and VBO
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Draw lines
        glDrawArrays(GL_LINES, 0, vertices.size() / 3);

        // Clean up
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void Renderer::beginUI() {
        // Disable depth testing for UI
        glDisable(GL_DEPTH_TEST);

        // Use UI shader
        Shader* shader = getShader("ui");
        if (!shader) return;

        shader->use();

        // Set orthographic projection
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_windowWidth),
            static_cast<float>(m_windowHeight), 0.0f, -1.0f, 1.0f);
        shader->setMat4("projection", projection);
    }

    void Renderer::endUI() {
        // Re-enable depth testing
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::drawText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
        // This is a placeholder - in a real implementation, you would use a font rendering system
        // For now, we'll just draw a rectangle for each character

        float charWidth = 8.0f * scale;
        float charHeight = 16.0f * scale;

        for (size_t i = 0; i < text.length(); i++) {
            drawRect(x + i * charWidth, y, charWidth, charHeight, glm::vec4(color, 1.0f));
        }
    }

    void Renderer::drawRect(float x, float y, float width, float height, const glm::vec4& color) {
        Shader* shader = getShader("ui");
        if (!shader) return;

        shader->use();
        shader->setBool("hasTexture", false);
        shader->setVec4("color", color);

        float vertices[] = {
            // positions        // texture coords
            x,          y,           0.0f, 0.0f,
            x + width,  y,           1.0f, 0.0f,
            x + width,  y + height,  1.0f, 1.0f,
            x,          y + height,  0.0f, 1.0f
        };

        glBindVertexArray(m_uiVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    void Renderer::drawLine2D(float x1, float y1, float x2, float y2, const glm::vec3& color, float thickness) {
        // Calculate perpendicular direction
        glm::vec2 dir(x2 - x1, y2 - y1);
        float length = glm::length(dir);

        if (length < 0.01f) return;

        dir /= length;
        glm::vec2 perp(-dir.y, dir.x);

        // Calculate corners of the line quad
        glm::vec2 offset = perp * (thickness * 0.5f);
        glm::vec2 p1 = glm::vec2(x1, y1) + offset;
        glm::vec2 p2 = glm::vec2(x1, y1) - offset;
        glm::vec2 p3 = glm::vec2(x2, y2) - offset;
        glm::vec2 p4 = glm::vec2(x2, y2) + offset;

        // Draw as a rectangle
        drawRect(p1.x, p1.y, p3.x - p1.x, p3.y - p1.y, glm::vec4(color, 1.0f));
    }

    GLFWwindow* Renderer::getWindow() const {
        return m_window;
    }

    int Renderer::getWindowWidth() const {
        return m_windowWidth;
    }

    int Renderer::getWindowHeight() const {
        return m_windowHeight;
    }

    void Renderer::setViewport(int width, int height) {
        m_windowWidth = width;
        m_windowHeight = height;
        glViewport(0, 0, width, height);
    }

    Shader* Renderer::getShader(const std::string& name) {
        auto it = m_shaders.find(name);
        if (it != m_shaders.end()) {
            return it->second;
        }
        return nullptr;
    }

    void Renderer::addShader(const std::string& name, Shader* shader) {
        if (m_shaders.find(name) != m_shaders.end()) {
            delete m_shaders[name];
        }
        m_shaders[name] = shader;
    }

} // namespace renderer

