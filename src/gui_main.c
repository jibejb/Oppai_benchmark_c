
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "benchmark.h"
#include "gui_main.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\0";

void gui_main() {
    Data *sharedData = malloc(sizeof(*sharedData));
    memset(sharedData, 0, sizeof(*sharedData));
    sharedData->t = 0.0;
    sharedData->S = 0.0;
    sharedData->Score = 0.0;
    sharedData->benchmark_running = 1;
    pthread_mutex_init(&sharedData->mutex, NULL);
    pthread_cond_init(&sharedData->cond, NULL);
    pthread_t benchmark_thread;
    if (pthread_create(&benchmark_thread, NULL, benchmark, (void*)sharedData) != 0) {
        perror("Failed to create main thread");
        exit(EXIT_FAILURE);
    }

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Oppai_benchmark_c", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    const int numVertices = 250;
    float vertices[2 * numVertices];
    while (sharedData->benchmark_running) {
        if (glfwWindowShouldClose(window)) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);

            glfwDestroyWindow(window);
            glfwTerminate();
            fprintf(stderr, "User canceled!\n");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_lock(&sharedData->mutex);
        pthread_cond_wait(&sharedData->cond, &sharedData->mutex);
        double temp_t = sharedData->t;
        double temp_S = sharedData->S;
        double temp_Score = sharedData->Score;
        pthread_mutex_unlock(&sharedData->mutex);
        fprintf(stdout, "S: %lf, Score: %lf\n", temp_S, temp_Score);
        generateVertices(vertices, numVertices, temp_t);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, numVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    if (pthread_join(benchmark_thread, NULL) != 0) {
        perror("Failed to join main thread");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void generateVertices(float* vertices, int numVertices, double t) {
    float xStart = -1.0f;
    float xEnd = 1.0f;
    float dx = (xEnd - xStart) / (numVertices - 1);
    double y = 0.0;
    for (int i = 0; i < numVertices; i++) {
        y = Oppai_func((double)i, t);
        float x = xStart + i * dx;
        vertices[2 * i] = y / (double)numVertices - 0.8;
        vertices[2 * i + 1] = x;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
