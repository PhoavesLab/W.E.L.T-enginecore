#include "mesh.h"
#include <glad/glad.h>

namespace renderer {

    Mesh::Mesh()
        : m_vao(0)
        , m_vbo(0)
        , m_ebo(0)
        , m_indexCount(0)
    {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
    }

    Mesh::~Mesh() {
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
        }

        if (m_vbo != 0) {
            glDeleteBuffers(1, &m_vbo);
        }

        if (m_ebo != 0) {
            glDeleteBuffers(1, &m_ebo);
        }
    }

    void Mesh::setVertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
        m_indexCount = indices.size();

        glBindVertexArray(m_vao);

        // Load vertex data
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Load index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void Mesh::draw() const {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    Mesh* Mesh::createCube(float size) {
        float halfSize = size / 2.0f;

        // Define the 8 vertices of the cube
        glm::vec3 v0(-halfSize, -halfSize, -halfSize);
        glm::vec3 v1(halfSize, -halfSize, -halfSize);
        glm::vec3 v2(halfSize, halfSize, -halfSize);
        glm::vec3 v3(-halfSize, halfSize, -halfSize);
        glm::vec3 v4(-halfSize, -halfSize, halfSize);
        glm::vec3 v5(halfSize, -halfSize, halfSize);
        glm::vec3 v6(halfSize, halfSize, halfSize);
        glm::vec3 v7(-halfSize, halfSize, halfSize);

        // Define normals for each face
        glm::vec3 frontNormal(0.0f, 0.0f, -1.0f);
        glm::vec3 backNormal(0.0f, 0.0f, 1.0f);
        glm::vec3 leftNormal(-1.0f, 0.0f, 0.0f);
        glm::vec3 rightNormal(1.0f, 0.0f, 0.0f);
        glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
        glm::vec3 topNormal(0.0f, 1.0f, 0.0f);

        // Create vertices with positions and normals
        std::vector<float> vertices = {
            // Front face
            v0.x, v0.y, v0.z, frontNormal.x, frontNormal.y, frontNormal.z,
            v1.x, v1.y, v1.z, frontNormal.x, frontNormal.y, frontNormal.z,
            v2.x, v2.y, v2.z, frontNormal.x, frontNormal.y, frontNormal.z,
            v3.x, v3.y, v3.z, frontNormal.x, frontNormal.y, frontNormal.z,

            // Back face
            v4.x, v4.y, v4.z, backNormal.x, backNormal.y, backNormal.z,
            v7.x, v7.y, v7.z, backNormal.x, backNormal.y, backNormal.z,
            v6.x, v6.y, v6.z, backNormal.x, backNormal.y, backNormal.z,
            v5.x, v5.y, v5.z, backNormal.x, backNormal.y, backNormal.z,

            // Left face
            v0.x, v0.y, v0.z, leftNormal.x, leftNormal.y, leftNormal.z,
            v3.x, v3.y, v3.z, leftNormal.x, leftNormal.y, leftNormal.z,
            v7.x, v7.y, v7.z, leftNormal.x, leftNormal.y, leftNormal.z,
            v4.x, v4.y, v4.z, leftNormal.x, leftNormal.y, leftNormal.z,

            // Right face
            v1.x, v1.y, v1.z, rightNormal.x, rightNormal.y, rightNormal.z,
            v5.x, v5.y, v5.z, rightNormal.x, rightNormal.y, rightNormal.z,
            v6.x, v6.y, v6.z, rightNormal.x, rightNormal.y, rightNormal.z,
            v2.x, v2.y, v2.z, rightNormal.x, rightNormal.y, rightNormal.z,

            // Bottom face
            v0.x, v0.y, v0.z, bottomNormal.x, bottomNormal.y, bottomNormal.z,
            v4.x, v4.y, v4.z, bottomNormal.x, bottomNormal.y, bottomNormal.z,
            v5.x, v5.y, v5.z, bottomNormal.x, bottomNormal.y, bottomNormal.z,
            v1.x, v1.y, v1.z, bottomNormal.x, bottomNormal.y, bottomNormal.z,

            // Top face
            v3.x, v3.y, v3.z, topNormal.x, topNormal.y, topNormal.z,
            v2.x, v2.y, v2.z, topNormal.x, topNormal.y, topNormal.z,
            v6.x, v6.y, v6.z, topNormal.x, topNormal.y, topNormal.z,
            v7.x, v7.y, v7.z, topNormal.x, topNormal.y, topNormal.z
        };

        // Create indices for the cube
        std::vector<unsigned int> indices = {
            // Front face
            0, 1, 2, 0, 2, 3,

            // Back face
            4, 5, 6, 4, 6, 7,

            // Left face
            8, 9, 10, 8, 10, 11,

            // Right face
            12, 13, 14, 12, 14, 15,

            // Bottom face
            16, 17, 18, 16, 18, 19,

            // Top face
            20, 21, 22, 20, 22, 23
        };

        Mesh* mesh = new Mesh();
        mesh->setVertices(vertices, indices);

        return mesh;
    }

    Mesh* Mesh::createGrid(int size, float cellSize) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float halfSize = size * cellSize / 2.0f;

        // Create grid lines along X and Z axes
        for (int i = -size / 2; i <= size / 2; i++) {
            float pos = i * cellSize;

            // Line along X axis
            vertices.push_back(-halfSize);
            vertices.push_back(0.0f);
            vertices.push_back(pos);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            vertices.push_back(halfSize);
            vertices.push_back(0.0f);
            vertices.push_back(pos);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            // Line along Z axis
            vertices.push_back(pos);
            vertices.push_back(0.0f);
            vertices.push_back(-halfSize);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            vertices.push_back(pos);
            vertices.push_back(0.0f);
            vertices.push_back(halfSize);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
        }

        // Create indices for the grid lines
        for (unsigned int i = 0; i < vertices.size() / 6; i++) {
            indices.push_back(i);
        }

        Mesh* mesh = new Mesh();
        mesh->setVertices(vertices, indices);

        return mesh;
    }

} // namespace renderer

