#ifndef GRAPH_H
#define GRAPH_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

struct Edge {
    uint32_t u;
    uint32_t v;
    uint8_t weight;
};

class Graph {
public:
    void addEdge(uint32_t u, uint32_t v, uint8_t weight);
    bool readFromTSV(const std::string& filename);
    bool writeToTSV(const std::string& filename) const;
    
    // Сериализация/десериализация
    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data);
    
    const std::vector<Edge>& getEdges() const { return edges_; }
    
private:
    std::vector<Edge> edges_;
    std::unordered_map<uint32_t, uint32_t> vertexMapping_;
    std::vector<uint32_t> originalVertices_;
    
    void buildVertexMapping();
};

#endif
