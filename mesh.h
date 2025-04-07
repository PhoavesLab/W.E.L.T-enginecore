#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace renderer {

    class Mesh {
    public:
        Mesh();
        ~Mesh();

        void setVertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        void draw() const;

        // Utility functions for creating common shapes
        static Mesh* createCube(float size = 1.0f);
        static Mesh* createGrid(int size, float cellSize);

    private:
        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ebo;
        unsigned int m_indexCount;
    };

} // namespace renderer

