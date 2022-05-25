#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <flecs.h>
#include <map>

#include "gl.h"
#include "shader.h"

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

    uint32_t depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    uint32_t depthMapFramebuff;
    glGenFramebuffers(1, &depthMapFramebuff);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuff);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uint32_t depthMapProgram = setupNewShaderProgram({ 
        std::make_pair(GL_VERTEX_SHADER, R"(
            #version 450 core

            layout (location = 0) in vec3 pos;

            uniform mat4 viewProj;
            uniform mat4 model;

            void main() {
                gl_Position = viewProj * model * vec4(pos, 1);
            }
        )"),
        std::make_pair(GL_FRAGMENT_SHADER, R"(
            #version 450 core

            void main() {
            }
        )")
    });

    uint32_t sceneProgram = setupNewShaderProgram({ 
        std::make_pair(GL_VERTEX_SHADER, R"(
            #version 450 core

            layout (location = 0) in vec3 pos;
            layout (location = 1) in vec3 normal;

            uniform mat4 proj;
            uniform mat4 view;
            uniform mat4 model;
            uniform mat4 lightViewProj;

            out Data {
                vec3 pos;
                vec3 normal;
                vec4 lightViewProjPos;
            } data;

            void main() {
                data.pos = vec3(model * vec4(pos, 1));
                data.normal = normal;
                data.lightViewProjPos = lightViewProj * vec4(data.pos, 1);
                gl_Position = proj * view * model * vec4(pos, 1);
            }
        )"), 
        std::make_pair(GL_FRAGMENT_SHADER, R"(
            #version 450 core

            uniform sampler2D shadowMap;

            in Data {
                vec3 pos;
                vec3 normal;
                vec4 lightViewProjPos;
            } data;

            out vec4 result;

            float calcShadow(vec4 pos) {
                vec3 p = pos.xyz / pos.w * .5 + .5;
                float d = texture(shadowMap, p.xy).r;
                float c = p.z;

                float bias = .005;
                return c - bias > d ? 1 : 0;
            }

            void main() {
                vec3 lightColor = vec3(1);
                vec3 lightDir = vec3(.25, .75, 0);
                vec3 ambient = .1 * lightColor;
                float diff = max(dot(data.normal, lightDir), 0);
                vec3 diffuse = diff * lightColor;

                float shadow = calcShadow(data.lightViewProjPos);
                vec3 lighting = ambient + (1 - shadow) * diffuse;

                result = vec4(lighting, 1);
            }
        )") 
    });

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

        // Depth map.

        const int viewsize = 10;
        const float aspect = size.x / size.y;
        glm::mat4 lightProj = glm::ortho(-20.f, 20.f, -20.f, 20.f, 0.f, 100.f);
        glm::mat4 lightView = glm::lookAt(glm::vec3(1, 4, -2), glm::vec3(0), glm::vec3(0, 1, 0));
        glm::mat4 lightMatrix = lightProj * lightView;

        glBindVertexArray(varr);
        glUseProgram(depthMapProgram);
        uniform(depthMapProgram, "viewProj", lightMatrix);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuff);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(depthMapProgram, "depthMap"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        for (int x = 0; x < 10; x++) {
            for (int z = 0; z < 10; z++) {
                uniform(depthMapProgram, "model", glm::translate(glm::mat4(1), glm::vec3(x, 0, z)));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
        for (int x = 0; x < 6; x++) {
            for (int z = 0; z < 6; z++) {
                uniform(depthMapProgram, "model", glm::translate(glm::mat4(1), glm::vec3(2 + x, 1, 2 + z)));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Scene.

        glBindVertexArray(varr);
        glUseProgram(sceneProgram);

        glViewport(0, 0, size.x, size.y);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(sceneProgram, "shadowMap"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        uniform(sceneProgram, "lightViewProj", lightMatrix);

        
        glm::mat4 proj = glm::ortho(aspect * -viewsize / 2, aspect * viewsize / 2, -viewsize / 2.f, viewsize / 2.f, -1000.f, 1000.f);
        uniform(sceneProgram, "proj", proj);

        glm::mat4 view = glm::lookAt(pos, pos - glm::vec3(1), glm::vec3(0, 1, 0));
        uniform(sceneProgram, "view", view);

        for (int x = 0; x < 10; x++) {
            for (int z = 0; z < 10; z++) {
                uniform(sceneProgram, "model", glm::translate(glm::mat4(1), glm::vec3(x, 0, z)));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }
        for (int x = 0; x < 6; x++) {
            for (int z = 0; z < 6; z++) {
                uniform(sceneProgram, "model", glm::translate(glm::mat4(1), glm::vec3(2 + x, 1, 2 + z)));
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}