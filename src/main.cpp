#include <iostream>
#include <glm/glm.hpp>
#include <flecs.h>
#include <map>

#include "gl.h"
#include "shader.h"
#include "renderer.h"

std::map<int, bool> keys;
glm::vec2 size;

int main() {
    keys[GLFW_KEY_LEFT] = false;
    keys[GLFW_KEY_RIGHT] = false;
    keys[GLFW_KEY_UP] = false;
    keys[GLFW_KEY_DOWN] = false;

    glfwInit();
    
    glfwSetErrorCallback([](int error, const char* desc) {
        std::cout << desc << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    size = glm::vec2(640, 400);
    GLFWwindow* window = glfwCreateWindow(size.x, size.y, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
    });

    glfwSetWindowSizeCallback(window, [](GLFWwindow*, int w, int h) {
        size.x = w;
        size.y = h;
    });

    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    });

    glEnable(GL_DEPTH_TEST);

    Renderer renderer;
    renderer.setup();

    std::vector<glm::vec3> cubes;

    for (int x = 0; x < 20; x++) {
        for (int z = 0; z < 20; z++) {
            cubes.emplace_back(glm::vec3(x, 0, z));
        }
    }
    for (int y = 1; y < 12; y++) {
        cubes.emplace_back(glm::vec3(5, y, 6));
    }

    glm::vec3 pos(10, 10, 10);

    while (!glfwWindowShouldClose(window)) {
        // Update.
        
        if (keys[GLFW_KEY_LEFT]) {
            pos.x -= 0.2;
        }
        if (keys[GLFW_KEY_RIGHT]) {
            pos.x += 0.2;
        }
        if (keys[GLFW_KEY_UP]) {
            pos.z -= 0.2;
        }
        if (keys[GLFW_KEY_DOWN]) {
            pos.z += 0.2;
        }

        // Render.

        renderer.render(cubes, pos, size.x, size.y);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}