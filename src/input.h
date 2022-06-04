#pragma once

#include <map>

#include "gl.h"

struct Input {
    std::map<int, bool> keys;

    void setup(GLFWwindow* window, GLFWUserPointers* userPointers) {
        userPointers->input = this;
        glfwSetWindowUserPointer(window, userPointers);

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            auto self = (Input*)((GLFWUserPointers*)glfwGetWindowUserPointer(window))->input;
            
            if (action == GLFW_PRESS) {
                self->keys[key] = true;
            }
            else if (action == GLFW_RELEASE) {
                self->keys[key] = false;
            }
        });
    }

    bool keyDown(int key) {
        if (keys.count(key) > 0) {
            return keys[key];
        }

        return false;
    }
};