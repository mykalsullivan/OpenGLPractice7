//
// Created by msullivan on 5/31/24.
//

#include <iostream>
#include <complex>
#include <cstring>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Window dimensions
const unsigned int WIDTH = 640, HEIGHT = 480;

// IDs for VAO, VBO, and shader objects
unsigned int vao, vbo, shader;

// Index buffer
unsigned int ibo;

// Uniform variables
unsigned int uniformModel, uniformProjection;
bool direction = true;
float triOffset = 0.0f, triMaxOffset = 1.0f, triTranslationIncrement = 0.015f;

// Angle stuff
float currentAngle = 0.0f;

// Vertex shader
static const char* vertexShader =
        "#version 330\n"
        "\n"
        "layout (location = 0) in vec3 pos;\n"                  /* Note: 'pos' is a built-in variable in OpenGL */
        "uniform mat4 model;\n"
        "uniform mat4 projection;\n"
        "\n"
        "out vec4 vertexColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * model * vec4(pos.x, pos.y, pos.z, 1.0);\n" /* Note: 'gl_Position' is also built-in */
        "   vertexColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);\n"        /* Note: 'clamp()' keeps values in a specified range */
        "}\n";

// Fragment shader
static const char* fragmentShader =
        "#version 330\n"
        "\n"
        "in vec4 vertexColor;\n"
        "\n"
        "out vec4 color;\n"                                     /* Note: color is built-in */
        "\n"
        "void main()\n"
        "{\n"
        "   color = vertexColor;\n"
        "}\n";

void createTriangle()
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

    // Generate and bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate, bind, and buffer index array
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate, bind, and buffer VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

void addShader(unsigned int program, const char* source, GLenum type)
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
    char errorLog[1024] = {};

    glGetShaderiv(newShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(newShader, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error compiling the " << type << " shader: " << errorLog << '\n';
        return;
    }

    // Attach new shader to the program
    glAttachShader(program, newShader);
}

void compileShaders()
{
    shader = glCreateProgram();

    if (!shader)
    {
        std::cout << "Error creating shader program\n";
        /* Note: Program could crash if shader fails to compile... so be more thorough here in the future */
        return;
    }

    addShader(shader, vertexShader, GL_VERTEX_SHADER);
    addShader(shader, fragmentShader, GL_FRAGMENT_SHADER);

    int result = 0;
    char errorLog[1024] = {};

    // Creates executable on GPU using shaders
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);

    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error linking program: " << errorLog << '\n';
        return;
    }

    // Ensures that the shader was created correctly
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error validating program: " << errorLog << '\n';
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model");
    uniformProjection = glGetUniformLocation(shader, "projection");
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

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    // Create main window
    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Perspective Projection Test", nullptr, nullptr);

    // Return if the main GLFW window cannot be created
    if (mainWindow == nullptr)
    {
        std::cout << "Could not create main GLFW window\n";
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = true;

    // Setup GLEW
    if (glewInit() == GLEW_OK)
    {
        std::cout << "Could not initialize GLEW\n";
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Used to prevent sides from being rendered incorrectly
    glEnable(GL_DEPTH_TEST);

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    // Create triangle and compile shaders
    createTriangle();
    compileShaders();

    glm::mat4 model(1.0f);
    glm::mat4 projection = glm::perspective(45.0f, (float) bufferWidth/(float) bufferHeight, 0.1f, 300.0f);

    // Move the pyramid away from the user
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-15.0f, -10.0f, -20.0f));

    // Main loop
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Set window size
        glfwGetWindowSize(mainWindow, &bufferWidth, &bufferHeight);
        glViewport(0, 0, bufferWidth, bufferHeight);

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

            glUseProgram(shader);

            std::cout << currentAngle << '\n';

            model = glm::rotate(model, 0.015f * (float) M_PI/2, glm::vec3(0.015f, 0.015f, 0.015f));
            model = glm::translate(model, glm::vec3(triOffset/5, 0.0f, 0.0f));
            //model = glm::rotate(model, currentAngle * (float) M_PI/180, glm::vec3(0.005f, 0.005f, 0.005f));

            glUniformMatrix4fv((int) uniformModel, 1, false, glm::value_ptr(model));
            glUniformMatrix4fv((int) uniformProjection, 1, false, glm::value_ptr(projection));

            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            glUseProgram(0);

            std::this_thread::sleep_for(std::chrono::microseconds(16667));
            i += 0.00005f;
            currentAngle += 0.05f;
        }

        // Swap display buffers
        glfwSwapBuffers(mainWindow);
    }

    // Delete main window
    glfwDestroyWindow(mainWindow);

    return 0;
}