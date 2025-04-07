#pragma once

#include "voxel_system.h"
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

namespace renderer {
    class Renderer;
    class Camera;
    class Mesh;
}

namespace voxel {

    class VoxelChunk;

    class VoxelWorld {
    public:
        VoxelWorld();
        ~VoxelWorld();

        bool initialize();
        void shutdown();
        void update(float deltaTime);
        void render(renderer::Renderer* renderer, renderer::Camera* camera);

        // Voxel manipulation
        bool addVoxel(int x, int y, int z);
        bool removeVoxel(int x, int y, int z);
        bool toggleVoxel(int x, int y, int z);
        bool hasVoxel(int x, int y, int z) const;

        // Raycast
        bool raycast(const glm::vec3& origin, const glm::vec3& direction,
            VoxelPos& hitPos, FaceDirection& hitFace, float maxDistance = 10.0f);

        // Chunk management
        VoxelChunk* getChunk(int chunkX, int chunkY, int chunkZ);
        VoxelChunk* getOrCreateChunk(int chunkX, int chunkY, int chunkZ);

        // Constants
        static const int CHUNK_SIZE = 16;

    private:
        // Convert world position to chunk coordinates
        void worldToChunkCoords(int worldX, int worldY, int worldZ,
            int& chunkX, int& chunkY, int& chunkZ,
            int& localX, int& localY, int& localZ) const;

        // Chunks storage
        std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, VoxelChunk*>>> m_chunks;
    };

} // namespace voxel

