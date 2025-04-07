#include "voxel_system.h"
#include "voxel_world.h"
#include "voxel_chunk.h"
#include "renderer.h"
#include "camera.h"
#include "mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace voxel {

    VoxelSystem::VoxelSystem()
        : m_world(nullptr)
        , m_gridMesh(nullptr)
    {
    }

    VoxelSystem::~VoxelSystem() {
        shutdown();
    }

    bool VoxelSystem::initialize() {
        // Create voxel world
        m_world = new VoxelWorld();
        if (!m_world->initialize()) {
            return false;
        }

        // Create grid mesh
        m_gridMesh = renderer::Mesh::createGrid(20, 1.0f);

        std::cout << "Voxel system initialized" << std::endl;
        return true;
    }

    void VoxelSystem::shutdown() {
        if (m_world) {
            delete m_world;
            m_world = nullptr;
        }

        if (m_gridMesh) {
            delete m_gridMesh;
            m_gridMesh = nullptr;
        }
    }

    void VoxelSystem::update(float deltaTime) {
        if (m_world) {
            m_world->update(deltaTime);
        }
    }

    void VoxelSystem::render(renderer::Renderer* renderer, renderer::Camera* camera) {
        if (!renderer || !camera) return;

        // Draw grid
        glm::mat4 gridModel = glm::mat4(1.0f);
        renderer->drawMesh(m_gridMesh, gridModel, glm::vec3(0.5f, 0.5f, 0.5f));

        // Draw voxel world
        if (m_world) {
            m_world->render(renderer, camera);
        }
    }

    bool VoxelSystem::addVoxel(int x, int y, int z) {
        if (m_world) {
            return m_world->addVoxel(x, y, z);
        }
        return false;
    }

    bool VoxelSystem::removeVoxel(int x, int y, int z) {
        if (m_world) {
            return m_world->removeVoxel(x, y, z);
        }
        return false;
    }

    bool VoxelSystem::toggleVoxel(int x, int y, int z) {
        if (m_world) {
            return m_world->toggleVoxel(x, y, z);
        }
        return false;
    }

    bool VoxelSystem::hasVoxel(int x, int y, int z) const {
        if (m_world) {
            return m_world->hasVoxel(x, y, z);
        }
        return false;
    }

    bool VoxelSystem::raycast(const glm::vec3& origin, const glm::vec3& direction,
        VoxelPos& hitPos, FaceDirection& hitFace, float maxDistance) {
        if (m_world) {
            return m_world->raycast(origin, direction, hitPos, hitFace, maxDistance);
        }
        return false;
    }

    VoxelWorld* VoxelSystem::getWorld() const {
        return m_world;
    }

} // namespace voxel

