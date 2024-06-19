#ifndef GUI_MAIN_H_INCLUDED
#define GUI_MAIN_H_INCLUDED

#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void generateVertices(float* vertices, int numVertices);
void gui_main();


#endif //GUI_MAIN_H_INCLUDED
