#include <iostream>

#include <flecs.h>

#include "gl.h"
#include "shader.h"
#include "renderer.h"
#include "input.h"
#include "window.h"
#include "asset.h"

int main() {
    glfwInit();
    
    glfwSetErrorCallback([](int error, const char* desc) {
        std::cout << desc << std::endl;
    });

    GLFWUserPointers userPointers;

    Window window;
    window.setup(&userPointers);

    Input input;
    input.setup(window.window, &userPointers);

    Renderer renderer;
    renderer.setup();

    glm::vec3 pos(10, 10, 10);

    while (!glfwWindowShouldClose(window.window)) {
        // Update.
        
        if (input.keyDown(GLFW_KEY_LEFT)) {
            pos.x -= 0.2;
        }
        if (input.keyDown(GLFW_KEY_RIGHT)) {
            pos.x += 0.2;
        }
        if (input.keyDown(GLFW_KEY_UP)) {
            pos.z -= 0.2;
        }
        if (input.keyDown(GLFW_KEY_DOWN)) {
            pos.z += 0.2;
        }

        // Render.

        renderer.render(pos, window.w, window.h);

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
}