#include "graph.h"
#include <cstring>  // для memcpy
#include <vector>
#include <cstdint>
#include <stdexcept>

// Функция для чтения VarInt
uint32_t readVarInt(const std::vector<uint8_t>& data, size_t& pos) {
    uint32_t result = 0;
    int shift = 0;

    while (true) {
        if (pos >= data.size()) throw std::runtime_error("Unexpected end of data");
        uint8_t byte = data[pos++];
        result |= (byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) break;
        shift += 7;
    }

    return result;
}

bool Graph::deserialize(const std::vector<uint8_t>& data) {
    edges_.clear();
    vertexMapping_.clear();
    originalVertices_.clear();

    if (data.size() < 14) return false;

    // Проверка магии и версии
    if (data[0] != 'G' || data[1] != 'R' || data[2] != 'A' ||
        data[3] != 'P' || data[4] != 'H') {
        return false;
    }

    uint8_t version = data[5];
    if (version != 3) return false; // Поддержка только версии 3

    size_t pos = 6;

    try {
        // Чтение кол-ва вершин и ребер через VarInt
        uint32_t vertexCount = readVarInt(data, pos);
        uint32_t edgeCount = readVarInt(data, pos);

        // Чтение вершин
        originalVertices_.resize(vertexCount);
        for (uint32_t i = 0; i < vertexCount; ++i) {
            originalVertices_[i] = readVarInt(data, pos);
            vertexMapping_[originalVertices_[i]] = i;
        }

        // Чтение ребер
        edges_.reserve(edgeCount);
        uint32_t current_u = 0;
        uint32_t current_v = 0;

        for (uint32_t i = 0; i < edgeCount; ++i) {
            if (pos >= data.size()) return false;

            // Чтение дельты и восстановление абсолютных значений
            current_u += readVarInt(data, pos);
            current_v += readVarInt(data, pos);

            if (pos >= data.size()) return false;
            uint8_t weight = data[pos++];

            // Восстановление оригинальных ID
            if (current_u < vertexCount && current_v < vertexCount) {
                edges_.push_back({originalVertices_[current_u], originalVertices_[current_v], weight});
            } else {
                return false;
            }
        }
    } catch (const std::exception&) {
        return false;
    }

    return true;
}
