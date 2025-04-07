#pragma once

#include <string>
#include <glm/glm.hpp>
#include "input_system.h" // Add this include

namespace renderer {
    class Renderer;
    class Mesh;
}

namespace game {

    class GameLayer;

    class GameObject {
    public:
        GameObject(const std::string& name);
        virtual ~GameObject();

        virtual bool initialize(GameLayer* gameLayer);
        virtual void shutdown();
        virtual void update(float deltaTime);
        virtual void render(renderer::Renderer* renderer);

        // Input handling
        virtual void handleKeyInput(int key, input::KeyState state);
        virtual void handleMouseInput(input::MouseButton button, bool pressed);

        // Getters and setters
        const std::string& getName() const;
        void setName(const std::string& name);

        const glm::vec3& getPosition() const;
        void setPosition(const glm::vec3& position);

        const glm::vec3& getRotation() const;
        void setRotation(const glm::vec3& rotation);

        const glm::vec3& getScale() const;
        void setScale(const glm::vec3& scale);

        glm::mat4 getModelMatrix() const;

    protected:
        std::string m_name;
        glm::vec3 m_position;
        glm::vec3 m_rotation; // Euler angles in degrees
        glm::vec3 m_scale;

        GameLayer* m_gameLayer;
    };

} // namespace game

