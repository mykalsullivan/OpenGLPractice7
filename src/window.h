//
// Created by msullivan on 6/4/24.
//

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
    GLWindow();
    GLWindow(float width, float height);
    ~GLWindow();
private:
    GLFWwindow* m_Window = nullptr;
    float m_BufferWidth, m_BufferHeight;
public:
    constexpr float getBufferWidth() const { return m_BufferWidth; }
    constexpr float getBufferHeight() const { return m_BufferHeight; }

    int init();
    bool shouldClose() { return glfwWindowShouldClose(m_Window); }
    void swapBuffers() { return glfwSwapBuffers(m_Window); }
};