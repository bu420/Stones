#include "asset.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <iostream>
#include <tuple>

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(const Vertex& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ hash<glm::vec3>()(vertex.norm) << 1) >> 1);
        }
    };
}

bool Vertex::operator==(const Vertex& other) const {
    return pos == other.pos && norm == other.norm;
}

std::vector<Vertex> Model::getVertices() const {
    return vertices;
}

std::vector<uint32_t> Model::getIndices() const {
    return indices;
}

std::vector<float> Model::plain() const {
    std::vector<float> plain;
    plain.reserve(vertices.size() * 6);

    for (const auto& vertex : vertices) {
        plain.emplace_back(vertex.pos.x);
        plain.emplace_back(vertex.pos.y);
        plain.emplace_back(vertex.pos.z);

        plain.emplace_back(vertex.norm.x);
        plain.emplace_back(vertex.norm.y);
        plain.emplace_back(vertex.norm.z);
    }

    return plain;
}

void Model::load(const std::string& path) {
    vertices.clear();
    indices.clear();
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        std::cout << warn << std::endl << err << std::endl;
        return;
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;

            vertex.pos.x = attrib.vertices.at(3 * index.vertex_index + 0);
            vertex.pos.y = attrib.vertices.at(3 * index.vertex_index + 1);
            vertex.pos.z = attrib.vertices.at(3 * index.vertex_index + 2);
            
            vertex.norm.x = attrib.normals.at(3 * index.normal_index + 0);
            vertex.norm.y = attrib.normals.at(3 * index.normal_index + 1);
            vertex.norm.z = attrib.normals.at(3 * index.normal_index + 2);
            
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = vertices.size();
                vertices.emplace_back(vertex);
            }
            
            indices.emplace_back(uniqueVertices.at(vertex));
        }
    }
}