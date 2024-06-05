//
// Created by msullivan on 6/4/24.
//

#include "mesh.h"

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_IBO(0), m_IndexCount(0)
{}

Mesh::~Mesh()
{
    clear();
}

void Mesh::create(float* vertices, unsigned int* indices, unsigned int vertexCount, unsigned int indexCount)
{
    m_IndexCount = indexCount;

    // Generate and bind VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Generate, bind, and buffer index array
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indexCount, indices, GL_STATIC_DRAW);

    // Generate, bind, and buffer VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertexCount, vertices, GL_STATIC_DRAW);

    /* index: Which vertex in buffer
     * size: Number of elements in buffer
     * type: Data type
     * normalized: If 0.0-0.1, then it is already normalized; if 0-255, then it is NOT normalized (I think)
     * stride: How many elements to skip
     * pointer: Where to start
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void Mesh::render()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::clear()
{
    if (m_IBO != 0)
    {
        glDeleteBuffers(1, &m_IBO);
        m_IBO = 0;
    }

    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    m_IndexCount = 0;
}