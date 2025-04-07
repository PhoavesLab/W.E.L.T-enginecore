#include "game_object.h"
#include "game_layer.h"
#include "renderer.h"
#include "input_system.h"
#include <glm/gtc/matrix_transform.hpp>

namespace game {

GameObject::GameObject(const std::string& name)
    : m_name(name)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_gameLayer(nullptr)
{
}

GameObject::~GameObject() {
    shutdown();
}

bool GameObject::initialize(GameLayer* gameLayer) {
    m_gameLayer = gameLayer;
    return true;
}

void GameObject::shutdown() {
    m_gameLayer = nullptr;
}

void GameObject::update(float deltaTime) {
    // Base implementation does nothing
}

void GameObject::render(renderer::Renderer* renderer) {
    // Base implementation does nothing
}

void GameObject::handleKeyInput(int key, input::KeyState state) {
    // Base implementation does nothing
}

void GameObject::handleMouseInput(input::MouseButton button, bool pressed) {
    // Base implementation does nothing
}

const std::string& GameObject::getName() const {
    return m_name;
}

void GameObject::setName(const std::string& name) {
    m_name = name;
}

const glm::vec3& GameObject::getPosition() const {
    return m_position;
}

void GameObject::setPosition(const glm::vec3& position) {
    m_position = position;
}

const glm::vec3& GameObject::getRotation() const {
    return m_rotation;
}

void GameObject::setRotation(const glm::vec3& rotation) {
    m_rotation = rotation;
}

const glm::vec3& GameObject::getScale() const {
    return m_scale;
}

void GameObject::setScale(const glm::vec3& scale) {
    m_scale = scale;
}

glm::mat4 GameObject::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    
    // Apply transformations in order: scale, rotate, translate
    model = glm::translate(model, m_position);
    
    // Apply rotations
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, m_scale);
    
    return model;
}

} // namespace game

