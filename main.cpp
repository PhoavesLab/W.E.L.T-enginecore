#include "engine_core.h"
#include "game_layer.h"
#include "example_object.h"
#include "engine_core.h"
#include "renderer.h"
#include "voxel_chunk.h"
#include "voxel_system.h"
#include "voxel_world.h"
#include "renderer.h"
#include "camera.h"
#include <iostream>
#include <memory>

int main() {
    try {
        // Create engine
        engine::EngineCore engine;

        // Initialize engine
        if (!engine.initialize(800, 600, "Voxel Engine")) {
            std::cerr << "Failed to initialize engine" << std::endl;
            return -1;
        }

        // Set initial camera position
        if (engine.getCamera()) {
            engine.getCamera()->setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
            engine.getCamera()->setRotation(-90.0f, 0.0f);  // Look along negative Z axis
        }

        // Create game layer
        auto gameLayer = std::make_unique<game::GameLayer>();
        if (!gameLayer->initialize(&engine)) {
            std::cerr << "Failed to initialize game layer" << std::endl;
            return -1;
        }

        // Create example object
        auto exampleObject = std::make_shared<game::ExampleObject>("Cube");
        exampleObject->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
        gameLayer->addGameObject(exampleObject);

        // Set up a simple voxel world
        auto voxelSystem = engine.getVoxelSystem();
        if (voxelSystem) {
            // Create a floor
            for (int x = -5; x <= 5; x++) {
                for (int z = -5; z <= 5; z++) {
                    voxelSystem->addVoxel(x, -1, z);
                }
            }

            // Create some simple structures
            voxelSystem->addVoxel(2, 0, 2);
            voxelSystem->addVoxel(2, 1, 2);
            voxelSystem->addVoxel(2, 2, 2);

            voxelSystem->addVoxel(-2, 0, -2);
            voxelSystem->addVoxel(-3, 0, -2);
            voxelSystem->addVoxel(-2, 0, -3);
            voxelSystem->addVoxel(-3, 0, -3);
            voxelSystem->addVoxel(-2, 1, -2);
        }

        // Print controls
        std::cout << "\n=== VOXEL ENGINE CONTROLS ===\n";
        std::cout << "  WASD - Move camera\n";
        std::cout << "  Mouse - Look around\n";
        std::cout << "  Space - Move up\n";
        std::cout << "  Shift - Move down\n";
        std::cout << "  Left Mouse Button - Add voxel\n";
        std::cout << "  Right Mouse Button - Remove voxel\n";
        std::cout << "  F - Toggle wireframe mode\n";
        std::cout << "  G - Toggle debug info\n";
        std::cout << "  1/2/3 - Change view mode\n";
        std::cout << "  ESC - Exit\n";
        std::cout << "============================\n\n";

        // Run the engine (this will block until the engine stops)
        engine.run();

        // Clean up
        gameLayer->shutdown();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return -1;
    }
}

