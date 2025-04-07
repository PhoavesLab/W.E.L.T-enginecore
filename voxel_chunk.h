#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace renderer {
    class Renderer;
    class Camera;
    class Mesh;
}

namespace voxel {

    class VoxelChunk {
    public:
        VoxelChunk(int chunkX, int chunkY, int chunkZ, int size);
        ~VoxelChunk();

        void update(float deltaTime);
        void render(renderer::Renderer* renderer, renderer::Camera* camera);

        // Voxel manipulation
        bool setVoxel(int x, int y, int z, bool value);
        bool hasVoxel(int x, int y, int z) const;
        bool isVoxelVisible(int x, int y, int z) const;

        // Chunk properties
        int getChunkX() const;
        int getChunkY() const;
        int getChunkZ() const;
        int getSize() const;

    private:
        void rebuildMesh();
        void createCubeFace(std::vector<float>& vertices, std::vector<unsigned int>& indices,
            int x, int y, int z, int faceIndex);

        int m_chunkX;
        int m_chunkY;
        int m_chunkZ;
        int m_size;

        // Voxel data
        std::vector<bool> m_voxels;

        // Mesh data
        renderer::Mesh* m_mesh;
        bool m_dirty;
    };

} // namespace voxel

