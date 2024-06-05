//
// Created by msullivan on 6/4/24.
//

#include "window.h"

#include <iostream>

GLWindow::GLWindow() : m_BufferWidth(800), m_BufferHeight(600)
{
    m_Window = glfwCreateWindow((int) m_BufferWidth, (int) m_BufferHeight, "OpenGL Practice", nullptr, nullptr);

    // Return if the main GLFW window cannot be created
    if (m_Window == nullptr)
    {
        std::cout << "Could not create main GLFW window\n";
        glfwTerminate();
        return;
    }
}

GLWindow::GLWindow(float width, float height) : m_BufferWidth(width), m_BufferHeight(height)
{
    m_Window = glfwCreateWindow((int) m_BufferWidth, (int) m_BufferHeight, "OpenGL Practice", nullptr, nullptr);

    // Return if the main GLFW window cannot be created
    if (m_Window == nullptr)
    {
        std::cout << "Could not create main GLFW window\n";
        glfwTerminate();
        return;
    }
}

GLWindow::~GLWindow()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

int GLWindow::init()
{
    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(m_Window, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(m_Window);

    return 0;
}