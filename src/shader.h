#pragma once

#include <glm/glm.hpp>
#include <string>
#include <initializer_list>
#include <utility>

using ShaderSource = std::pair<uint32_t, std::string>;

void compile(uint32_t shader, const std::string& source);
uint32_t setupNewShaderProgram(std::initializer_list<ShaderSource> sources);
void uniform(uint32_t program, const std::string& location, const glm::mat4& value);
