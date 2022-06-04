#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "gl.h"

struct Vertex {
    glm::vec3 pos, norm;

    bool operator==(const Vertex& other) const;
};

class Model {
public:
    std::vector<Vertex> getVertices() const;
    std::vector<uint32_t> getIndices() const;
    std::vector<float> plain() const;

    void load(const std::string& path);

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};