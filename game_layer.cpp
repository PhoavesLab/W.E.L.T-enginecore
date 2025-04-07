#include "game_layer.h"
#include "game_object.h"
#include "viewer.h"
#include "engine_core.h"
#include "input_system.h"
#include "voxel_system.h"
#include "debug_system.h"
#include "renderer.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

namespace game {

    GameLayer::GameLayer()
        : m_engineCore(nullptr)
    {
    }

    GameLayer::~GameLayer() {
        shutdown();
    }

    bool GameLayer::initialize(engine::EngineCore* engineCore) {
        m_engineCore = engineCore;

        if (!m_engineCore) {
            std::cerr << "Failed to initialize game layer: Engine core is null" << std::endl;
            return false;
        }

        // Create viewer
        m_viewer = std::make_unique<Viewer>();
        if (!m_viewer->initialize(m_engineCore)) {
            std::cerr << "Failed to initialize viewer" << std::endl;
            return false;
        }

        // Set up input handlers
        setupInputHandlers();

        std::cout << "Game layer initialized" << std::endl;
        return true;
    }

    void GameLayer::shutdown() {
        // Clear game objects
        m_gameObjects.clear();

        // Shutdown viewer
        if (m_viewer) {
            m_viewer->shutdown();
        }

        m_engineCore = nullptr;
    }

    void GameLayer::update(float deltaTime) {
        // Update viewer
        if (m_viewer) {
            m_viewer->update(deltaTime);
        }

        // Update game objects
        for (auto& gameObject : m_gameObjects) {
            gameObject->update(deltaTime);
        }
    }

    void GameLayer::render() {
        // Render game objects
        for (auto& gameObject : m_gameObjects) {
            gameObject->render(m_engineCore->getRenderer());
        }

        // Render viewer
        if (m_viewer) {
            m_viewer->render();
        }
    }

    void GameLayer::addGameObject(std::shared_ptr<GameObject> gameObject) {
        if (gameObject) {
            // Initialize the game object
            gameObject->initialize(this);

            // Add to list
            m_gameObjects.push_back(gameObject);
        }
    }

    void GameLayer::removeGameObject(const std::string& name) {
        auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
            [&name](const std::shared_ptr<GameObject>& obj) {
                return obj->getName() == name;
            });

        if (it != m_gameObjects.end()) {
            m_gameObjects.erase(it);
        }
    }

    std::shared_ptr<GameObject> GameLayer::getGameObject(const std::string& name) {
        auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
            [&name](const std::shared_ptr<GameObject>& obj) {
                return obj->getName() == name;
            });

        if (it != m_gameObjects.end()) {
            return *it;
        }

        return nullptr;
    }

    Viewer* GameLayer::getViewer() const {
        return m_viewer.get();
    }

    engine::EngineCore* GameLayer::getEngineCore() const {
        return m_engineCore;
    }

    void GameLayer::setupInputHandlers() {
        if (!m_engineCore) return;

        input::InputSystem* inputSystem = m_engineCore->getInputSystem();
        if (!inputSystem) return;

        // Register key callback
        inputSystem->registerKeyCallback("GameLayer", [this](int key, input::KeyState state) {
            this->handleKeyInput(key, state);
            });

        // Register mouse button callback
        inputSystem->registerMouseButtonCallback("GameLayer", [this](input::MouseButton button, bool pressed) {
            this->handleMouseInput(button, pressed);
            });
    }

    void GameLayer::handleKeyInput(int key, input::KeyState state) {
        // Handle game-specific key input
        if (state == input::KeyState::PRESSED) {
            switch (key) {
            case GLFW_KEY_ESCAPE:
                // Exit the game
                if (m_engineCore) {
                    m_engineCore->stop();
                }
                break;

            case GLFW_KEY_F:
                // Toggle wireframe mode
                if (m_engineCore && m_engineCore->getRenderer()) {
                    bool wireframe = m_engineCore->getRenderer()->isWireframeMode();
                    m_engineCore->getRenderer()->setWireframeMode(!wireframe);
                }
                break;

            case GLFW_KEY_G:
                // Toggle debug info
                if (m_engineCore && m_engineCore->getDebugSystem() &&
                    m_engineCore->getDebugSystem()->getViewer()) {
                    bool show = m_engineCore->getDebugSystem()->getViewer()->isShowingPerformanceMetrics();
                    m_engineCore->getDebugSystem()->getViewer()->setShowPerformanceMetrics(!show);
                }
                break;
            }
        }

        // Pass input to viewer
        if (m_viewer) {
            m_viewer->handleKeyInput(key, state);
        }

        // Pass input to game objects
        for (auto& gameObject : m_gameObjects) {
            gameObject->handleKeyInput(key, state);
        }
    }

    void GameLayer::handleMouseInput(input::MouseButton button, bool pressed) {
        // Handle game-specific mouse input
        if (pressed) {
            // Only process on press, not release
            if (m_engineCore && m_engineCore->getVoxelSystem() && m_engineCore->getCamera()) {
                glm::vec3 cameraPos = m_engineCore->getCamera()->getPosition();
                glm::vec3 cameraDir = m_engineCore->getCamera()->getFront();

                voxel::VoxelPos hitPos;
                voxel::FaceDirection hitFace;

                if (m_engineCore->getVoxelSystem()->raycast(cameraPos, cameraDir, hitPos, hitFace, 10.0f)) {
                    if (button == input::MouseButton::LEFT) {
                        // Add voxel adjacent to the hit face
                        int newX = hitPos.x;
                        int newY = hitPos.y;
                        int newZ = hitPos.z;

                        // Adjust position based on hit face
                        switch (hitFace) {
                        case voxel::FaceDirection::FRONT:
                            newZ -= 1;
                            break;
                        case voxel::FaceDirection::BACK:
                            newZ += 1;
                            break;
                        case voxel::FaceDirection::LEFT:
                            newX -= 1;
                            break;
                        case voxel::FaceDirection::RIGHT:
                            newX += 1;
                            break;
                        case voxel::FaceDirection::BOTTOM:
                            newY -= 1;
                            break;
                        case voxel::FaceDirection::TOP:
                            newY += 1;
                            break;
                        }

                        // Add the voxel
                        bool success = m_engineCore->getVoxelSystem()->addVoxel(newX, newY, newZ);
                        if (success) {
                            std::cout << "Added voxel at (" << newX << ", " << newY << ", " << newZ << ")" << std::endl;
                        }
                    }
                    else if (button == input::MouseButton::RIGHT) {
                        // Remove the hit voxel
                        bool success = m_engineCore->getVoxelSystem()->removeVoxel(hitPos.x, hitPos.y, hitPos.z);
                        if (success) {
                            std::cout << "Removed voxel at (" << hitPos.x << ", " << hitPos.y << ", " << hitPos.z << ")" << std::endl;
                        }
                    }
                }
            }
        }

        // Pass input to viewer
        if (m_viewer) {
            m_viewer->handleMouseInput(button, pressed);
        }

        // Pass input to game objects
        for (auto& gameObject : m_gameObjects) {
            gameObject->handleMouseInput(button, pressed);
        }
    }

} // namespace game

