#include <iostream>

#include <flecs.h>

#include "gl.h"
#include "shader.h"
#include "renderer.h"
#include "input.h"
#include "window.h"
#include "asset.h"
#include "camera.h"

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

    Camera camera;
    camera.setup({ 0, 0, 0 });

    glm::vec3 pos(10, 10, 10);

    while (!glfwWindowShouldClose(window.window)) {
        // Update.

        camera.update();

        if (input.keyDown(GLFW_KEY_LEFT)) {
            camera.rotate(-1, 0);
        }
        if (input.keyDown(GLFW_KEY_RIGHT)) {
            camera.rotate(1, 0);
        }
        if (input.keyDown(GLFW_KEY_UP)) {
            camera.rotate(0, 1);
        }
        if (input.keyDown(GLFW_KEY_DOWN)) {
            camera.rotate(0, -1);
        }
        
        if (input.keyDown(GLFW_KEY_A)) {
            camera.moveX(-.1);
        }
        if (input.keyDown(GLFW_KEY_D)) {
            camera.moveX(.1);
        }
        if (input.keyDown(GLFW_KEY_W)) {
            camera.moveZ(-.1);
        }
        if (input.keyDown(GLFW_KEY_S)) {
            camera.moveZ(.1);
        }
        if (input.keyDown(GLFW_KEY_SPACE)) {
            camera.moveY(.1);
        }
        if (input.keyDown(GLFW_KEY_LEFT_SHIFT)) {
            camera.moveY(-.1);
        }

        // Render.

        renderer.render(camera, window.w, window.h);

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
}