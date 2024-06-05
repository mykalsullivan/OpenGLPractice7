//
// Created by msullivan on 6/4/24.
//

#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;
private:
    unsigned int m_ID, m_UniformProjection, m_UniformModel;
private:
    void compile(const char* vertexSource, const char* fragmentSource);
    static void add(unsigned int program, const char* shaderSource, GLenum shaderType);
    static std::string readFile(const char* path);
public:
    void createFromStrings(const char* vertexSource, const char* fragmentSource);
    void createFromFiles(const char* vertexSourceFile, const char* fragmentSourceFile);
    constexpr unsigned int getProjectionLocation() const { return m_UniformProjection; }
    constexpr unsigned int getModelLocation() const { return m_UniformModel; }
    void use();
    void clear();
};