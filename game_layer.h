#pragma once

#include <vector>
#include <memory>
#include <string>
#include "engine_core.h"
#include "input_system.h" // Add this include

namespace game {

    class GameObject;
    class Viewer;

    class GameLayer {
    public:
        GameLayer();
        ~GameLayer();

        bool initialize(engine::EngineCore* engineCore);
        void shutdown();

        void update(float deltaTime);
        void render();

        // Game object management
        void addGameObject(std::shared_ptr<GameObject> gameObject);
        void removeGameObject(const std::string& name);
        std::shared_ptr<GameObject> getGameObject(const std::string& name);

        // Viewer access
        Viewer* getViewer() const;

        // Engine core access
        engine::EngineCore* getEngineCore() const;

    private:
        engine::EngineCore* m_engineCore;
        std::vector<std::shared_ptr<GameObject>> m_gameObjects;
        std::unique_ptr<Viewer> m_viewer;

        // Input handling
        void setupInputHandlers();
        void handleKeyInput(int key, input::KeyState state);
        void handleMouseInput(input::MouseButton button, bool pressed);
    };

} // namespace game

