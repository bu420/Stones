#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <flecs.h>
#include <map>

#include "gl.h"

void compile(uint32_t shader, const std::string& src) {
    auto temp = src.c_str();
    glShaderSource(shader, 1, &temp, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cout << log << std::endl;
    }
}

void uniform(uint32_t program, const std::string& location, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(program, location.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

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

    glEnable(GL_DEPTH_FUNC);

    const float cube[] = {
        0, 0, 0,    0, 0, -1,
        0, 1, 0,    0, 0, -1,
        1, 1, 0,    0, 0, -1,
        1, 0, 0,    0, 0, -1,

        0, 0, 1,    0, 0, 1,
        0, 1, 1,    0, 0, 1,
        1, 1, 1,    0, 0, 1,
        1, 0, 1,    0, 0, 1,

        0, 0, 0,    -1, 0, 0,
        0, 1, 0,    -1, 0, 0,
        0, 1, 1,    -1, 0, 0,
        0, 0, 1,    -1, 0, 0,

        1, 0, 0,    1, 0, 0,
        1, 1, 0,    1, 0, 0,
        1, 1, 1,    1, 0, 0,
        1, 0, 1,    1, 0, 0,

        0, 0, 0,    0, -1, 0,
        1, 0, 0,    0, -1, 0,
        1, 0, 1,    0, -1, 0,
        0, 0, 1,    0, -1, 0,

        0, 1, 0,    0, 1, 0,
        1, 1, 0,    0, 1, 0,
        1, 1, 1,    0, 1, 0,
        0, 1, 1,    0, 1, 0
    };

    uint32_t indices[36];
    const uint32_t pattern[6] = { 0, 1, 2,    2, 3, 0 };
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            indices[i * 6 + j] = i * 4 + pattern[j];
        }
    }

    uint32_t vbuff;
    glGenBuffers(1, &vbuff);

    uint32_t ebuff;
    glGenBuffers(1, &ebuff);

    uint32_t varr;
    glGenVertexArrays(1, &varr);

    glBindVertexArray(varr);
    glBindBuffer(GL_ARRAY_BUFFER, vbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Position.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    // Normal.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    const std::string vshaderSrc = R"(
        #version 450 core

        layout (location = 0) in vec3 pos;
        layout (location = 1) in vec3 normal;

        out vec3 p;
        out vec3 n;

        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 model;

        void main() {
            p = vec3(model * vec4(pos, 1));
            n = normal;
            gl_Position = proj * view * vec4(p, 1);
        }
    )";

    const std::string fshaderSrc = R"(
        #version 450 core

        in vec3 p;
        in vec3 n;

        out vec4 result;

        void main() {
            vec3 lightColor = vec3(1);
            vec3 lightDir = normalize(vec3(.5, 1, 0));
            vec3 ambient = .1 * lightColor;
            float diff = max(dot(n, lightDir), 0);
            vec3 diffuse = diff * lightColor;

            result = vec4(ambient + diffuse, 1);
        }
    )";

    uint32_t vshader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fshader = glCreateShader(GL_FRAGMENT_SHADER);
    compile(vshader, vshaderSrc);
    compile(fshader, fshaderSrc);
    uint32_t program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    glUseProgram(program);

    glm::vec3 pos(100, 100, 100);

    while (!glfwWindowShouldClose(window)) {
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

        const int viewsize = 10;
        const float aspect = size.x / size.y;
        glm::mat4 proj = glm::ortho(aspect * -viewsize / 2, aspect * viewsize / 2, -viewsize / 2.f, viewsize / 2.f, -1000.f, 1000.f);
        uniform(program, "proj", proj);

        glm::mat4 view = glm::lookAt(pos, pos - glm::vec3(1), glm::vec3(0, 1, 0));
        uniform(program, "view", view);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int x = 0; x < 10; x++) {
            for (int z = 0; z < 10; z++) {
                uniform(program, "model", glm::translate(glm::mat4(1), glm::vec3(x, 0, z)));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
        for (int x = 0; x < 6; x++) {
            for (int z = 0; z < 6; z++) {
                uniform(program, "model", glm::translate(glm::mat4(1), glm::vec3(2 + x, 1, 2 + z)));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}