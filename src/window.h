#pragma once

#include <glm/glm.hpp>

#include "gl.h"

struct Window {
    GLFWwindow* window;
    uint32_t w, h;

    void setup(GLFWUserPointers* userPointers) {
        w = 640;
        h = 400;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(w, h, "", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        userPointers->window = this;
        glfwSetWindowUserPointer(window, userPointers);

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) {
            auto self = (Window*)((GLFWUserPointers*)glfwGetWindowUserPointer(window))->window;

            self->w = w;
            self->h = h;
        });
    }
};