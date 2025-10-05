#include "graph.h"
#include <cstring>  // для memcpy
#include <vector>
#include <cstdint>
#include <stdexcept>

bool Graph::deserialize(const std::vector<uint8_t>& data) {
    edges_.clear();
    vertexMapping_.clear();
    originalVertices_.clear();

    if (data.size() < 14) return false; // Минимальный размер заголовка

    // Проверка магии и версии
    if (data[0] != 'G' || data[1] != 'R' || data[2] != 'A' ||
        data[3] != 'P' || data[4] != 'H' || data[5] != 1) {
        return false;
    }

    size_t pos = 6;

    // Чтение кол-ва вершин и ребер
    if (pos + 8 > data.size()) return false;

    uint32_t vertexCount, edgeCount;
    memcpy(&vertexCount, &data[pos], 4);
    pos += 4;
    memcpy(&edgeCount, &data[pos], 4);
    pos += 4;

    // Чтение вершин
    originalVertices_.resize(vertexCount);
    if (pos + vertexCount * 4 > data.size()) return false;

    for (uint32_t i = 0; i < vertexCount; ++i) {
        memcpy(&originalVertices_[i], &data[pos], 4);
        vertexMapping_[originalVertices_[i]] = i;
        pos += 4;
    }

    // Восстанавление ребер из списков смежности
    for (uint32_t i = 0; i < vertexCount; ++i) {
        if (pos + 4 > data.size()) return false;

        uint32_t neighborCount;
        memcpy(&neighborCount, &data[pos], 4);
        pos += 4;

        uint32_t current_vertex = i;
        for (uint32_t j = 0; j < neighborCount; ++j) {
            if (pos >= data.size()) return false;

            // Чтение дельты
            uint32_t delta;
            uint8_t first_byte = data[pos++];

            if ((first_byte & 0x80) == 0) {
                delta = first_byte;
            } else if ((first_byte & 0xC0) == 0x80) {
                if (pos >= data.size()) return false;
                delta = ((first_byte & 0x3F) << 8) | data[pos++];
            } else {
                if (pos + 4 > data.size()) return false;
                memcpy(&delta, &data[pos], 4);
                pos += 4;
            }

            if (pos >= data.size()) return false;
            uint8_t weight = data[pos++];

            current_vertex += delta;
            if (current_vertex >= vertexCount) return false;

            // Добавление ребра (только в одном направлении, чтобы избежать дубликатов)
            if (i <= current_vertex) {
                edges_.push_back({originalVertices_[i], originalVertices_[current_vertex], weight});
            }
        }
    }

    return true;
}
