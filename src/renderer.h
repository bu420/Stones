#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gl.h"
#include "shader.h"
#include "asset.h"

struct Renderer {
    uint32_t depthMap, depthFramebuff;
    uint32_t depthProgram;

    uint32_t modelVertexbuff, modelElementbuff, modelVertexarr;
    uint32_t modelProgram;

    Model model;

    void setup() {
        model.load("asset/Residential Buildings 001.obj");

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenFramebuffers(1, &depthFramebuff);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuff);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenBuffers(1, &modelVertexbuff);
        glGenBuffers(1, &modelElementbuff);
        glGenVertexArrays(1, &modelVertexarr);

        const auto vertices = model.getVertices();
        const auto indices = model.getIndices();

        glBindVertexArray(modelVertexarr);
        glBindBuffer(GL_ARRAY_BUFFER, modelVertexbuff);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelElementbuff);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        // Position.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        // Normal.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

        depthProgram = setupNewShaderProgram({ 
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

        modelProgram = setupNewShaderProgram({ 
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

                    float bias = .008;
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
    }

    void renderScene(uint32_t program) {
        uniform(program, "model", glm::mat4(1));
        glDrawElements(GL_TRIANGLES, model.getIndices().size(), GL_UNSIGNED_INT, 0);
    }

    void render(const glm::vec3& eye, int w, int h) {
        glEnable(GL_DEPTH_TEST);

        // Render scene to depth map.

        const float shadowMapSize = 50;
        glm::mat4 lightProj = glm::ortho(-shadowMapSize, shadowMapSize, -shadowMapSize, shadowMapSize, 0.f, 100.f);
        glm::mat4 lightView = glm::lookAt(glm::vec3(1, 4, -2), glm::vec3(0), glm::vec3(0, 1, 0));
        glm::mat4 lightMatrix = lightProj * lightView;

        glBindVertexArray(modelVertexarr);
        glUseProgram(depthProgram);
        uniform(depthProgram, "viewProj", lightMatrix);

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuff);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(depthProgram, "depthMap"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        renderScene(depthProgram);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render scene normally.

        glBindVertexArray(modelVertexarr);
        glUseProgram(modelProgram);

        glViewport(0, 0, w, h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(modelProgram, "shadowMap"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        uniform(modelProgram, "lightViewProj", lightMatrix);

        glm::mat4 proj = glm::perspective(glm::radians(80.f), w / (float)h, 0.01f, 1000.f);
        uniform(modelProgram, "proj", proj);

        glm::mat4 view = glm::lookAt(eye, eye - glm::vec3(1), glm::vec3(0, 1, 0));
        uniform(modelProgram, "view", view);

        renderScene(modelProgram);
    }
};