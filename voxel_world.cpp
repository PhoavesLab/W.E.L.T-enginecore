#include "voxel_world.h"
#include "voxel_chunk.h"
#include "renderer.h"
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

namespace voxel {

    VoxelWorld::VoxelWorld() {
    }

    VoxelWorld::~VoxelWorld() {
        shutdown();
    }

    bool VoxelWorld::initialize() {
        // Create a few initial chunks
        getOrCreateChunk(0, 0, 0);

        // Add some initial voxels
        addVoxel(0, 0, 0);
        addVoxel(1, 0, 0);
        addVoxel(0, 1, 0);

        std::cout << "Voxel world initialized" << std::endl;
        return true;
    }

    void VoxelWorld::shutdown() {
        // Delete all chunks
        for (auto& xMap : m_chunks) {
            for (auto& yMap : xMap.second) {
                for (auto& chunk : yMap.second) {
                    delete chunk.second;
                }
            }
        }

        m_chunks.clear();
    }

    void VoxelWorld::update(float deltaTime) {
        // Update all chunks
        for (auto& xMap : m_chunks) {
            for (auto& yMap : xMap.second) {
                for (auto& chunk : yMap.second) {
                    chunk.second->update(deltaTime);
                }
            }
        }
    }

    void VoxelWorld::render(renderer::Renderer* renderer, renderer::Camera* camera) {
        if (!renderer || !camera) return;

        // Render all chunks
        for (auto& xMap : m_chunks) {
            for (auto& yMap : xMap.second) {
                for (auto& chunk : yMap.second) {
                    chunk.second->render(renderer, camera);
                }
            }
        }
    }

    bool VoxelWorld::addVoxel(int x, int y, int z) {
        int chunkX, chunkY, chunkZ, localX, localY, localZ;
        worldToChunkCoords(x, y, z, chunkX, chunkY, chunkZ, localX, localY, localZ);

        VoxelChunk* chunk = getOrCreateChunk(chunkX, chunkY, chunkZ);
        if (chunk) {
            return chunk->setVoxel(localX, localY, localZ, true);
        }

        return false;
    }

    bool VoxelWorld::removeVoxel(int x, int y, int z) {
        int chunkX, chunkY, chunkZ, localX, localY, localZ;
        worldToChunkCoords(x, y, z, chunkX, chunkY, chunkZ, localX, localY, localZ);

        VoxelChunk* chunk = getChunk(chunkX, chunkY, chunkZ);
        if (chunk) {
            return chunk->setVoxel(localX, localY, localZ, false);
        }

        return false;
    }

    bool VoxelWorld::toggleVoxel(int x, int y, int z) {
        if (hasVoxel(x, y, z)) {
            return removeVoxel(x, y, z);
        }
        else {
            return addVoxel(x, y, z);
        }
    }

    bool VoxelWorld::hasVoxel(int x, int y, int z) const {
        int chunkX, chunkY, chunkZ, localX, localY, localZ;
        worldToChunkCoords(x, y, z, chunkX, chunkY, chunkZ, localX, localY, localZ);

        // Find chunk
        auto xIt = m_chunks.find(chunkX);
        if (xIt == m_chunks.end()) return false;

        auto yIt = xIt->second.find(chunkY);
        if (yIt == xIt->second.end()) return false;

        auto zIt = yIt->second.find(chunkZ);
        if (zIt == yIt->second.end()) return false;

        return zIt->second->hasVoxel(localX, localY, localZ);
    }

    bool VoxelWorld::raycast(const glm::vec3& origin, const glm::vec3& direction,
        VoxelPos& hitPos, FaceDirection& hitFace, float maxDistance) {
        // Implementation of a fast voxel traversal algorithm
        // Based on "A Fast Voxel Traversal Algorithm for Ray Tracing"

        // Normalize direction
        glm::vec3 dir = glm::normalize(direction);

        // Current voxel position (starting at the origin)
        int x = floor(origin.x);
        int y = floor(origin.y);
        int z = floor(origin.z);

        // Direction sign (1 or -1)
        int stepX = (dir.x > 0) ? 1 : ((dir.x < 0) ? -1 : 0);
        int stepY = (dir.y > 0) ? 1 : ((dir.y < 0) ? -1 : 0);
        int stepZ = (dir.z > 0) ? 1 : ((dir.z < 0) ? -1 : 0);

        // Distance to next voxel boundary
        float tMaxX = (stepX > 0) ? (ceil(origin.x) - origin.x) / dir.x :
            (stepX < 0) ? (floor(origin.x) - origin.x) / dir.x : FLT_MAX;
        float tMaxY = (stepY > 0) ? (ceil(origin.y) - origin.y) / dir.y :
            (stepY < 0) ? (floor(origin.y) - origin.y) / dir.y : FLT_MAX;
        float tMaxZ = (stepZ > 0) ? (ceil(origin.z) - origin.z) / dir.z :
            (stepZ < 0) ? (floor(origin.z) - origin.z) / dir.z : FLT_MAX;

        // Distance to next voxel boundary (constant)
        float tDeltaX = (stepX != 0) ? 1.0f / fabs(dir.x) : FLT_MAX;
        float tDeltaY = (stepY != 0) ? 1.0f / fabs(dir.y) : FLT_MAX;
        float tDeltaZ = (stepZ != 0) ? 1.0f / fabs(dir.z) : FLT_MAX;

        // Distance traveled
        float t = 0.0f;

        // Avoid starting inside a voxel
        if (hasVoxel(x, y, z)) {
            hitPos = { x, y, z };

            // Determine hit face based on ray direction
            if (fabs(dir.x) > fabs(dir.y) && fabs(dir.x) > fabs(dir.z)) {
                hitFace = (dir.x > 0) ? FaceDirection::LEFT : FaceDirection::RIGHT;
            }
            else if (fabs(dir.y) > fabs(dir.z)) {
                hitFace = (dir.y > 0) ? FaceDirection::BOTTOM : FaceDirection::TOP;
            }
            else {
                hitFace = (dir.z > 0) ? FaceDirection::FRONT : FaceDirection::BACK;
            }

            return true;
        }

        // Traverse the voxel grid
        while (t < maxDistance) {
            // Move to the next voxel
            if (tMaxX < tMaxY && tMaxX < tMaxZ) {
                t = tMaxX;
                x += stepX;
                tMaxX += tDeltaX;
                hitFace = (stepX > 0) ? FaceDirection::LEFT : FaceDirection::RIGHT;
            }
            else if (tMaxY < tMaxZ) {
                t = tMaxY;
                y += stepY;
                tMaxY += tDeltaY;
                hitFace = (stepY > 0) ? FaceDirection::BOTTOM : FaceDirection::TOP;
            }
            else {
                t = tMaxZ;
                z += stepZ;
                tMaxZ += tDeltaZ;
                hitFace = (stepZ > 0) ? FaceDirection::FRONT : FaceDirection::BACK;
            }

            // Check if we hit a voxel
            if (hasVoxel(x, y, z)) {
                hitPos = { x, y, z };
                return true;
            }
        }

        // No hit found within max distance
        return false;
    }

    VoxelChunk* VoxelWorld::getChunk(int chunkX, int chunkY, int chunkZ) {
        auto xIt = m_chunks.find(chunkX);
        if (xIt == m_chunks.end()) return nullptr;

        auto yIt = xIt->second.find(chunkY);
        if (yIt == xIt->second.end()) return nullptr;

        auto zIt = yIt->second.find(chunkZ);
        if (zIt == yIt->second.end()) return nullptr;

        return zIt->second;
    }

    VoxelChunk* VoxelWorld::getOrCreateChunk(int chunkX, int chunkY, int chunkZ) {
        // Check if chunk already exists
        VoxelChunk* chunk = getChunk(chunkX, chunkY, chunkZ);
        if (chunk) return chunk;

        // Create new chunk
        chunk = new VoxelChunk(chunkX, chunkY, chunkZ, CHUNK_SIZE);
        m_chunks[chunkX][chunkY][chunkZ] = chunk;

        return chunk;
    }

    void VoxelWorld::worldToChunkCoords(int worldX, int worldY, int worldZ,
        int& chunkX, int& chunkY, int& chunkZ,
        int& localX, int& localY, int& localZ) const {
        // Handle negative coordinates correctly
        chunkX = (worldX < 0 && worldX % CHUNK_SIZE != 0) ? (worldX / CHUNK_SIZE - 1) : (worldX / CHUNK_SIZE);
        chunkY = (worldY < 0 && worldY % CHUNK_SIZE != 0) ? (worldY / CHUNK_SIZE - 1) : (worldY / CHUNK_SIZE);
        chunkZ = (worldZ < 0 && worldZ % CHUNK_SIZE != 0) ? (worldZ / CHUNK_SIZE - 1) : (worldZ / CHUNK_SIZE);

        // Calculate local coordinates
        localX = worldX - chunkX * CHUNK_SIZE;
        localY = worldY - chunkY * CHUNK_SIZE;
        localZ = worldZ - chunkZ * CHUNK_SIZE;
    }

} // namespace voxel

