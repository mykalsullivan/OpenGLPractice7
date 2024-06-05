//
// Created by msullivan on 6/4/24.
//

#pragma once
#include <GL/glew.h>

class Mesh
{
private:
    unsigned int m_VAO, m_VBO, m_IBO;
    size_t m_IndexCount;
public:
    Mesh();
    ~Mesh();

    void create(float* vertices, unsigned int* indices, unsigned int vertexCount, unsigned int indexCount);
    void render();
    void clear();
};