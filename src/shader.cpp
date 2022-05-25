#include "shader.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "gl.h"

void compile(uint32_t shader, const std::string& source) {
    auto temp = source.c_str();
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

uint32_t setupNewShaderProgram(std::initializer_list<ShaderSource> sources) {
    uint32_t program = glCreateProgram();
    
    for (auto& source : sources) {
        uint32_t shader = glCreateShader(source.first);
        compile(shader, source.second);
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    return program;
}

void uniform(uint32_t program, const std::string& location, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(program, location.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}