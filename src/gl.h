#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct GLFWUserPointers {
    void* window;
    void* input;
};
