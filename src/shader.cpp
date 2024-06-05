//
// Created by msullivan on 6/4/24.
//

#include "shader.h"

#include <iostream>
#include <cstring>
#include <fstream>

void Shader::compile(const char* vertexSource, const char* fragmentSource)
{
    // Create a shader program and get ID
    m_ID = glCreateProgram();

    // Check for shader creation errors
    if (!m_ID)
    {
        std::cout << "Failed to create shader program\n";
        return;
    }

    add(m_ID, vertexSource, GL_VERTEX_SHADER);
    add(m_ID, fragmentSource, GL_FRAGMENT_SHADER);

    // Link shader program
    glLinkProgram(m_ID);

    /* Used to check for errors */
    int result = 0;
    char errorMessage[1024] {};

    // Check for linking errors
    glGetProgramiv(m_ID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(m_ID, sizeof(errorMessage), nullptr, errorMessage);
        std::cout << "Failed to link shader program: " << errorMessage << '\n';
        return;
    }

    // Validate shader program
    glValidateProgram(m_ID);

    // Check for validation errors
    glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(m_ID, sizeof(errorMessage), nullptr, errorMessage);
        std::cout << "Failed to validate shader program: " << errorMessage << '\n';
        return;
    }

    // Set uniform location IDs
    m_UniformProjection = glGetUniformLocation(m_ID, "projection");
    m_UniformModel = glGetUniformLocation(m_ID, "model");
}

void Shader::add(unsigned int program, const char* source, GLenum type)
{
    unsigned int newShader = glCreateShader(type);
    const char* theCode[1];
    theCode[0] = source;

    int codeLength[1];
    codeLength[0] = (int) strlen(source);

    /* shader: Shader ID
     * count: # of shaders being created (I think)
     * string: Source code
     * length: Length of source code
     */
    glShaderSource(newShader, 1, theCode, codeLength);
    glCompileShader(newShader);

    int result = 0;
    char errorMessage[1024] = {};

    // Check for compilation errors
    glGetShaderiv(newShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(newShader, sizeof(errorMessage), nullptr, errorMessage);
        std::cout << "Error compiling the " << type << " shader: " << errorMessage;
        return;
    }

    // Attach new shader to the shader program
    glAttachShader(program, newShader);
}

std::string Shader::readFile(const char* path)
{
    std::string fileContent;
    std::ifstream infile(path, std::ios::in);

    if (!infile.is_open())
    {
        std::cout << "Failed to read file from path \"" << path << "\"\n";
        return "";
    }

    std::string line;
    while (!infile.eof())
    {
        std::getline(infile, line);
        fileContent.append(line + '\n');
    }
    return fileContent;
}

void Shader::use()
{
    glUseProgram(m_ID);
}

void Shader::clear()
{
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
        m_ID = 0;
    }

    m_UniformModel = 0;
    m_UniformProjection = 0;
}

void Shader::createFromStrings(const char* vertexSource, const char* fragmentSource)
{
    compile(vertexSource, fragmentSource);
}

void Shader::createFromFiles(const char* vertexSourceFile, const char* fragmentSourceFile)
{
    std::string vertexSource = readFile(vertexSourceFile),
                fragmentSource = readFile(fragmentSourceFile);
    createFromStrings(vertexSource.c_str(), fragmentSource.c_str());
}