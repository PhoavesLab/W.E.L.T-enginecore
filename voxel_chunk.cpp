#include "voxel_chunk.h"
#include "renderer.h"
#include "camera.h"
#include "mesh.h"
#include <glm/gtc/matrix_transform.hpp>

namespace voxel {

    VoxelChunk::VoxelChunk(int chunkX, int chunkY, int chunkZ, int size)
        : m_chunkX(chunkX)
        , m_chunkY(chunkY)
        , m_chunkZ(chunkZ)
        , m_size(size)
        , m_mesh(nullptr)
        , m_dirty(true)
    {
        // Initialize voxel data
        m_voxels.resize(size * size * size, false);
    }

    VoxelChunk::~VoxelChunk() {
        if (m_mesh) {
            delete m_mesh;
        }
    }

    void VoxelChunk::update(float deltaTime) {
        // Rebuild mesh if dirty
        if (m_dirty) {
            rebuildMesh();
            m_dirty = false;
        }
    }

    void VoxelChunk::render(renderer::Renderer* renderer, renderer::Camera* camera) {
        if (!renderer || !camera || !m_mesh) return;

        // Calculate model matrix
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(m_chunkX * m_size, m_chunkY * m_size, m_chunkZ * m_size)
        );

        // Draw mesh with a more vibrant color
        renderer->drawMesh(m_mesh, model, glm::vec3(0.9f, 0.5f, 0.2f));
    }

    bool VoxelChunk::setVoxel(int x, int y, int z, bool value) {
        if (x < 0 || y < 0 || z < 0 || x >= m_size || y >= m_size || z >= m_size) {
            return false;
        }

        int index = (z * m_size * m_size) + (y * m_size) + x;

        if (m_voxels[index] != value) {
            m_voxels[index] = value;
            m_dirty = true;
            return true;
        }

        return false;
    }

    bool VoxelChunk::hasVoxel(int x, int y, int z) const {
        if (x < 0 || y < 0 || z < 0 || x >= m_size || y >= m_size || z >= m_size) {
            return false;
        }

        int index = (z * m_size * m_size) + (y * m_size) + x;
        return m_voxels[index];
    }

    bool VoxelChunk::isVoxelVisible(int x, int y, int z) const {
        // Check if voxel exists
        if (!hasVoxel(x, y, z)) {
            return false;
        }

        // Check if any face is visible (not occluded by another voxel)
        return !hasVoxel(x - 1, y, z) || // Left face
            !hasVoxel(x + 1, y, z) || // Right face
            !hasVoxel(x, y - 1, z) || // Bottom face
            !hasVoxel(x, y + 1, z) || // Top face
            !hasVoxel(x, y, z - 1) || // Front face
            !hasVoxel(x, y, z + 1);   // Back face
    }

    int VoxelChunk::getChunkX() const {
        return m_chunkX;
    }

    int VoxelChunk::getChunkY() const {
        return m_chunkY;
    }

    int VoxelChunk::getChunkZ() const {
        return m_chunkZ;
    }

    int VoxelChunk::getSize() const {
        return m_size;
    }

    void VoxelChunk::rebuildMesh() {
        // Clean up old mesh
        if (m_mesh) {
            delete m_mesh;
            m_mesh = nullptr;
        }

        // Create new mesh
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Add faces for each visible voxel
        for (int z = 0; z < m_size; z++) {
            for (int y = 0; y < m_size; y++) {
                for (int x = 0; x < m_size; x++) {
                    if (hasVoxel(x, y, z)) {
                        // Add faces that are not occluded
                        if (!hasVoxel(x, y, z - 1)) { // Front face
                            createCubeFace(vertices, indices, x, y, z, 0);
                        }
                        if (!hasVoxel(x, y, z + 1)) { // Back face
                            createCubeFace(vertices, indices, x, y, z, 1);
                        }
                        if (!hasVoxel(x - 1, y, z)) { // Left face
                            createCubeFace(vertices, indices, x, y, z, 2);
                        }
                        if (!hasVoxel(x + 1, y, z)) { // Right face
                            createCubeFace(vertices, indices, x, y, z, 3);
                        }
                        if (!hasVoxel(x, y - 1, z)) { // Bottom face
                            createCubeFace(vertices, indices, x, y, z, 4);
                        }
                        if (!hasVoxel(x, y + 1, z)) { // Top face
                            createCubeFace(vertices, indices, x, y, z, 5);
                        }
                    }
                }
            }
        }

        // Create mesh if there are any vertices
        if (!vertices.empty()) {
            m_mesh = new renderer::Mesh();
            m_mesh->setVertices(vertices, indices);
        }
    }

    void VoxelChunk::createCubeFace(std::vector<float>& vertices, std::vector<unsigned int>& indices,
        int x, int y, int z, int faceIndex) {
        // Define the 8 vertices of the cube
        glm::vec3 v0(x, y, z);
        glm::vec3 v1(x + 1, y, z);
        glm::vec3 v2(x + 1, y + 1, z);
        glm::vec3 v3(x, y + 1, z);
        glm::vec3 v4(x, y, z + 1);
        glm::vec3 v5(x + 1, y, z + 1);
        glm::vec3 v6(x + 1, y + 1, z + 1);
        glm::vec3 v7(x, y + 1, z + 1);

        // Define normals for each face
        glm::vec3 normals[] = {
            glm::vec3(0.0f, 0.0f, -1.0f), // Front
            glm::vec3(0.0f, 0.0f, 1.0f),  // Back
            glm::vec3(-1.0f, 0.0f, 0.0f), // Left
            glm::vec3(1.0f, 0.0f, 0.0f),  // Right
            glm::vec3(0.0f, -1.0f, 0.0f), // Bottom
            glm::vec3(0.0f, 1.0f, 0.0f)   // Top
        };

        // Get the current vertex count
        unsigned int baseIndex = vertices.size() / 6;

        // Add vertices and indices for the selected face
        switch (faceIndex) {
        case 0: // Front face (negative z)
            vertices.insert(vertices.end(), {
                v0.x, v0.y, v0.z, normals[0].x, normals[0].y, normals[0].z,
                v1.x, v1.y, v1.z, normals[0].x, normals[0].y, normals[0].z,
                v2.x, v2.y, v2.z, normals[0].x, normals[0].y, normals[0].z,
                v3.x, v3.y, v3.z, normals[0].x, normals[0].y, normals[0].z
                });
            break;
        case 1: // Back face (positive z)
            vertices.insert(vertices.end(), {
                v4.x, v4.y, v4.z, normals[1].x, normals[1].y, normals[1].z,
                v7.x, v7.y, v7.z, normals[1].x, normals[1].y, normals[1].z,
                v6.x, v6.y, v6.z, normals[1].x, normals[1].y, normals[1].z,
                v5.x, v5.y, v5.z, normals[1].x, normals[1].y, normals[1].z
                });
            break;
        case 2: // Left face (negative x)
            vertices.insert(vertices.end(), {
                v0.x, v0.y, v0.z, normals[2].x, normals[2].y, normals[2].z,
                v3.x, v3.y, v3.z, normals[2].x, normals[2].y, normals[2].z,
                v7.x, v7.y, v7.z, normals[2].x, normals[2].y, normals[2].z,
                v4.x, v4.y, v4.z, normals[2].x, normals[2].y, normals[2].z
                });
            break;
        case 3: // Right face (positive x)
            vertices.insert(vertices.end(), {
                v1.x, v1.y, v1.z, normals[3].x, normals[3].y, normals[3].z,
                v5.x, v5.y, v5.z, normals[3].x, normals[3].y, normals[3].z,
                v6.x, v6.y, v6.z, normals[3].x, normals[3].y, normals[3].z,
                v2.x, v2.y, v2.z, normals[3].x, normals[3].y, normals[3].z
                });
            break;
        case 4: // Bottom face (negative y)
            vertices.insert(vertices.end(), {
                v0.x, v0.y, v0.z, normals[4].x, normals[4].y, normals[4].z,
                v4.x, v4.y, v4.z, normals[4].x, normals[4].y, normals[4].z,
                v5.x, v5.y, v5.z, normals[4].x, normals[4].y, normals[4].z,
                v1.x, v1.y, v1.z, normals[4].x, normals[4].y, normals[4].z
                });
            break;
        case 5: // Top face (positive y)
            vertices.insert(vertices.end(), {
                v3.x, v3.y, v3.z, normals[5].x, normals[5].y, normals[5].z,
                v2.x, v2.y, v2.z, normals[5].x, normals[5].y, normals[5].z,
                v6.x, v6.y, v6.z, normals[5].x, normals[5].y, normals[5].z,
                v7.x, v7.y, v7.z, normals[5].x, normals[5].y, normals[5].z
                });
            break;
        }

        // Add indices for the quad (two triangles)
        indices.insert(indices.end(), {
            baseIndex, baseIndex + 1, baseIndex + 2,
            baseIndex, baseIndex + 2, baseIndex + 3
            });
    }

} // namespace voxel

