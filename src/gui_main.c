
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "benchmark.h"
#include "gui_main.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in float aPos;\n"
    "uniform float u_time;\n"
    "uniform float numVertices;\n"
    "float oppai_func(float y, float t) {\n"
    "	y = 0.02 * (y - 100.0);\n"
    "\n"
    "	float a1 = (1.5 * exp((0.12*sin(t)-0.5)*pow((y+0.16*sin(t)), 2.0))) / (1.0 + exp(-20.0*(5.0*y+sin(t))));\n"
    "	float a2 = ((1.5 + 0.8*pow((y+0.2*sin(t)), 3.0)) * pow(1.0+exp(20.0*(5.0*y+sin(t))), -1.0)) / (1.0 + exp(-(100.0*(y+1.0) + 16.0*sin(t))));\n"
    "	float a3 = (0.2 * (exp(-pow(y+1.0, 2.0)) + 1.0)) / (1.0 + exp(100.0*(y+1.0)+16.0*sin(t)));\n"
    "	float a4 = 0.1 / exp(2.0*pow((10.0*y+1.2*(2.0+sin(t))*sin(t)), 4.0));\n"
    "\n"
    "	return 65.0 * (a1 + a2 + a3 + a4);\n"
    "}\n"
    "\n"
     "vec2 processVertex(float position) {\n"
    "	float dx = 2.0 / (numVertices-1.0);\n"
    "	float x = -1.0 + position * dx;\n"
    "	float y = oppai_func(position, u_time) / numVertices - 0.8;\n"
    "  return vec2(y, x);\n"
    "}\n"
    "\n"
   "void main() {\n"
    "   gl_Position = vec4(processVertex(aPos), 0.0, 1.0);\n"
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
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    const int numVertices = 250;
    float vertices[numVertices];
    generateVertices(vertices, numVertices);
    char title[64];

    double time0 = glfwGetTime();
    double dt;
    double dframe = -1;
    double fps = 0;
    double time1 = 0;

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

        time1 = glfwGetTime();
        dframe++;
        if ((dt = time1 - time0) >= 1) {
            fps = dframe / dt;
            time0 = time1;
            dframe = 0;
        }
        sprintf(title, "Area: %.2lf, Score: %.2lf, FPS: %2lf\n", temp_S, temp_Score, fps);
        glfwSetWindowTitle(window, title);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int uTimeLocation = glGetUniformLocation(shaderProgram, "u_time");
        glUniform1f(uTimeLocation, temp_t);

        int numVerticesLocation = glGetUniformLocation(shaderProgram, "numVertices");
        glUniform1f(numVerticesLocation, (float)numVertices);

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

void generateVertices(float* vertices, int numVertices) {
    for (int i = 0; i < numVertices; i++) {
        vertices[i] = (float)i;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        ;
}

