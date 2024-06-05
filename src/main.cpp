//
// Created by msullivan on 5/31/24.
//

#include <iostream>
#include <memory>
#include <complex>
#include <vector>
#include <cstring>
#include <thread>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "mesh.h"
#include "shader.h"

namespace
{
    unsigned int uniformProjection = 0, uniformModel = 0;
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Shader>> shaders;

    // Shader stuff
    const char* vertexShader = "/home/msullivan/Projects/CLion/OpenGLPractice7/src/Shaders/shader.vertex";
    const char* fragmentShader = "/home/msullivan/Projects/CLion/OpenGLPractice7/src/Shaders/shader.fragment";
}

// Uniform variables
bool direction = true;
float triOffset = 0.0f, triMaxOffset = 1.0f, triTranslationIncrement = 0.015f;
float currentAngle = 0.0f;

void createObjects()
{
    unsigned int indices[] = {
            0, 3, 1,
            1, 3, 2,
            2, 3, 0,
            0, 1, 2
    };

    // Create vertex coordinates
    float vertices[] = {
            -1.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    mesh->create(vertices, indices, 12, 12);
    meshes.emplace_back(mesh);
}

void createShaders()
{
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();
    shader->createFromFiles(vertexShader, fragmentShader);
    shaders.emplace_back(shader);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "GLFW failed to initialize\n";

        // Unload GLFW memory
        glfwTerminate();
        return 1;
    }

    GLWindow window(800, 600);
    window.init();

    // Allow modern extension features
    glewExperimental = true;

    // Setup GLEW
    if (glewInit() == GLEW_OK)
    {
        std::cout << "Could not initialize GLEW\n";
        glfwTerminate();
        return 1;
    }

    // Used to prevent sides from being rendered incorrectly
    glEnable(GL_DEPTH_TEST);

    // Setup viewport size
    glViewport(0, 0, window.getBufferWidth(), window.getBufferHeight());

    createObjects();
    createShaders();

    glm::mat4 projection = glm::perspective(45.0f, window.getBufferHeight()/window.getBufferWidth(), 0.1f, 300.0f);
    glm::mat4 model(1.0f);

    model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
    model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -10.0f));


    // Main loop
    while (!window.shouldClose())
    {
        // Get/handle user input
        glfwPollEvents();

        {
            static float i = 0;

//            // Rotates between "hard" RGB values
//            auto r = (float) std::sin(i+(2*M_PI/3));
//            auto g = (float) std::sin(i);
//            auto b = (float) std::sin(i-(2*M_PI/3));

            // Rotates between RGB values, but with a LOT of color blending
            auto r = (float) std::abs(std::sin(i+(2*M_PI/3)));
            auto g = (float) std::abs(std::sin(i));
            auto b = (float) std::abs(std::sin(i-(2*M_PI/3)));

            if (direction)
            {
                triOffset += triTranslationIncrement;
            }
            else
            {
                triOffset -= triTranslationIncrement;
            }

            if (std::abs(triOffset) >= triMaxOffset)
            {
                direction = !direction;
            }

            if (currentAngle >= 360)
            {
                currentAngle -= 360;
            }

            // Clear window
            glClearColor(r, g, b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaders[0]->use();
            uniformProjection = shaders[0]->getProjectionLocation();
            uniformModel = shaders[0]->getModelLocation();

            model = glm::translate(model, glm::vec3(triOffset/10, 0.0f, -0.0f));
            glUniformMatrix4fv((int) uniformModel, 1, false, glm::value_ptr(model));
            glUniformMatrix4fv((int) uniformProjection, 1, false, glm::value_ptr(projection));

            for (const auto& mesh : meshes) mesh->render();

            glUseProgram(0);

            std::this_thread::sleep_for(std::chrono::microseconds(16667));
            i += 0.015f;
            currentAngle += 0.05f;
        }

        // Swap display buffers
        window.swapBuffers();
    }
    return 0;
}