#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

namespace renderer {
    class Renderer;
    class Camera;
    class Mesh;
}

namespace voxel {

    // Voxel position structure
    struct VoxelPos {
        int x, y, z;

        bool operator==(const VoxelPos& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    // Face direction enum
    enum class FaceDirection {
        FRONT,  // -Z
        BACK,   // +Z
        LEFT,   // -X
        RIGHT,  // +X
        BOTTOM, // -Y
        TOP     // +Y
    };

    // Hash function for VoxelPos
    struct VoxelPosHash {
        size_t operator()(const VoxelPos& pos) const {
            return ((pos.x * 73856093) ^ (pos.y * 19349663) ^ (pos.z * 83492791)) % 10000000;
        }
    };

    // Forward declarations
    class VoxelWorld;
    class VoxelChunk;

    class VoxelSystem {
    public:
        VoxelSystem();
        ~VoxelSystem();

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

        // World access
        VoxelWorld* getWorld() const;

    private:
        VoxelWorld* m_world;
        renderer::Mesh* m_gridMesh;
    };

} // namespace voxel

