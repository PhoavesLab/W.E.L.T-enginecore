#pragma once

#include "game_object.h"
#include <memory>

namespace renderer {
    class Mesh;
}

namespace game {

class ExampleObject : public GameObject {
public:
    ExampleObject(const std::string& name);
    virtual ~ExampleObject();

    virtual bool initialize(GameLayer* gameLayer) override;
    virtual void shutdown() override;
    virtual void update(float deltaTime) override;
    virtual void render(renderer::Renderer* renderer) override;
    
    // Input handling
    virtual void handleKeyInput(int key, input::KeyState state) override;
    virtual void handleMouseInput(input::MouseButton button, bool pressed) override;
    
    // Object properties
    void setColor(const glm::vec3& color);
    const glm::vec3& getColor() const;
    
private:
    std::unique_ptr<renderer::Mesh> m_mesh;
    glm::vec3 m_color;
    
    // Animation properties
    bool m_isAnimating;
    float m_animationTime;
    float m_rotationSpeed;
    float m_bobSpeed;
    float m_bobHeight;
};

} // namespace game

