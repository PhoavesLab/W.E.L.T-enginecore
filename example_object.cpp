#include "example_object.h"
#include "game_layer.h"
#include "renderer.h"
#include "mesh.h"
#include "input_system.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace game {

ExampleObject::ExampleObject(const std::string& name)
    : GameObject(name)
    , m_color(0.8f, 0.2f, 0.2f)
    , m_isAnimating(true)
    , m_animationTime(0.0f)
    , m_rotationSpeed(45.0f)
    , m_bobSpeed(2.0f)
    , m_bobHeight(0.5f)
{
}

ExampleObject::~ExampleObject() {
    shutdown();
}

bool ExampleObject::initialize(GameLayer* gameLayer) {
    if (!GameObject::initialize(gameLayer)) {
        return false;
    }
    
    // Create mesh
    m_mesh.reset(renderer::Mesh::createCube(1.0f));
    
    std::cout << "Example object initialized: " << m_name << std::endl;
    return true;
}

void ExampleObject::shutdown() {
    m_mesh.reset();
    GameObject::shutdown();
}

void ExampleObject::update(float deltaTime) {
    // Update animation
    if (m_isAnimating) {
        m_animationTime += deltaTime;
        
        // Rotate around Y axis
        m_rotation.y += m_rotationSpeed * deltaTime;
        
        // Bob up and down
        float bobOffset = sin(m_animationTime * m_bobSpeed) * m_bobHeight;
        m_position.y = 1.0f + bobOffset; // Base height + bob offset
    }
}

void ExampleObject::render(renderer::Renderer* renderer) {
    if (!renderer || !m_mesh) return;
    
    // Get model matrix
    glm::mat4 modelMatrix = getModelMatrix();
    
    // Draw mesh
    renderer->drawMesh(m_mesh.get(), modelMatrix, m_color);
}

void ExampleObject::handleKeyInput(int key, input::KeyState state) {
    if (state != input::KeyState::PRESSED) return;
    
    switch (key) {
        case GLFW_KEY_R:
            // Toggle animation
            m_isAnimating = !m_isAnimating;
            std::cout << "Animation " << (m_isAnimating ? "enabled" : "disabled") << " for " << m_name << std::endl;
            break;
            
        case GLFW_KEY_C:
            // Change color
            m_color = glm::vec3(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX
            );
            std::cout << "Changed color of " << m_name << std::endl;
            break;
    }
}

void ExampleObject::handleMouseInput(input::MouseButton button, bool pressed) {
    // No specific mouse handling for this object
}

void ExampleObject::setColor(const glm::vec3& color) {
    m_color = color;
}

const glm::vec3& ExampleObject::getColor() const {
    return m_color;
}

} // namespace game

