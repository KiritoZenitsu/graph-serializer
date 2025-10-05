#include "graph.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cstring>

void Graph::addEdge(uint32_t u, uint32_t v, uint8_t weight) {
    edges_.push_back({u, v, weight});
}

bool Graph::readFromTSV(const std::string& filename) {
    edges_.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        uint32_t u, v;
        int weight;

        if (!(iss >> u >> v >> weight)) {
            std::cerr << "Invalid line format: " << line << std::endl;
            return false;
        }

        if (weight < 0 || weight > 255) {
            std::cerr << "Weight out of range: " << weight << std::endl;
            return false;
        }

        edges_.push_back({u, v, static_cast<uint8_t>(weight)});
    }

    buildVertexMapping();
    return true;
}

bool Graph::writeToTSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot create file: " << filename << std::endl;
        return false;
    }

    for (const auto& edge : edges_) {
        file << edge.u << '\t' << edge.v << '\t' << static_cast<int>(edge.weight) << '\n';
    }

    return true;
}

void Graph::buildVertexMapping() {
    std::vector<uint32_t> vertices;
    for (const auto& edge : edges_) {
        vertices.push_back(edge.u);
        vertices.push_back(edge.v);
    }

    // Удаление дубликатов
    std::sort(vertices.begin(), vertices.end());
    vertices.erase(std::unique(vertices.begin(), vertices.end()), vertices.end());

    // Создание mapping
    vertexMapping_.clear();
    originalVertices_ = vertices;

    for (size_t i = 0; i < vertices.size(); ++i) {
        vertexMapping_[vertices[i]] = i;
    }
}
