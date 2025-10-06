#include "graph.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cstring>

// Функция для VarInt кодирования (сжатие чисел)
void writeVarInt(std::vector<uint8_t>& result, uint32_t value) {
    while (value >= 0x80) {
        result.push_back(static_cast<uint8_t>(value | 0x80));
        value >>= 7;
    }
    result.push_back(static_cast<uint8_t>(value));
}

std::vector<uint8_t> Graph::serialize() const {
    std::vector<uint8_t> result;

    // Заголовок: магия + версия
    const char magic[] = "GRAPH";
    result.insert(result.end(), magic, magic + 5);
    result.push_back(3); // версия 3 - с VarInt кодированием

    // Кол-во вершин и ребер
    uint32_t vertexCount = originalVertices_.size();
    uint32_t edgeCount = edges_.size();

    // Использование VarInt для размеров
    writeVarInt(result, vertexCount);
    writeVarInt(result, edgeCount);

    // Запись вершины (оригинальные ID) с VarInt
    for (uint32_t vertex : originalVertices_) {
        writeVarInt(result, vertex);
    }

    // Запись ребра с VarInt кодированием
    uint32_t prev_u = 0;
    uint32_t prev_v = 0;

    for (const auto& edge : edges_) {
        uint32_t new_u = vertexMapping_.at(edge.u);
        uint32_t new_v = vertexMapping_.at(edge.v);

        // Запись всегда в порядке min-max
        if (new_u > new_v) {
            std::swap(new_u, new_v);
        }

        // Дельта-кодирование для вершин
        writeVarInt(result, new_u - prev_u);
        writeVarInt(result, new_v - prev_v);
        result.push_back(edge.weight);

        prev_u = new_u;
        prev_v = new_v;
    }

    return result;
}
