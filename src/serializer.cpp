#include "graph.h"
#include <vector>
#include <cstdint>
#include <algorithm>

std::vector<uint8_t> Graph::serialize() const {
    std::vector<uint8_t> result;

    // Заголовок: магия + версия
    const char magic[] = "GRAPH";
    result.insert(result.end(), magic, magic + 5);
    result.push_back(1); // Версия

    // Кол-во вершин и ребер
    uint32_t vertexCount = originalVertices_.size();
    uint32_t edgeCount = edges_.size();

    // Запись кол-ва вершин и ребер
    result.insert(result.end(), reinterpret_cast<const uint8_t*>(&vertexCount),
                 reinterpret_cast<const uint8_t*>(&vertexCount) + 4);
    result.insert(result.end(), reinterpret_cast<const uint8_t*>(&edgeCount),
                 reinterpret_cast<const uint8_t*>(&edgeCount) + 4);

    // Запись вершин (ориг ID)
    for (uint32_t vertex : originalVertices_) {
        result.insert(result.end(), reinterpret_cast<const uint8_t*>(&vertex),
                     reinterpret_cast<const uint8_t*>(&vertex) + 4);
    }

    // Создание списка смежности с новыми индексами
    std::vector<std::vector<std::pair<uint32_t, uint8_t>>> adjacency(vertexCount);
    for (const auto& edge : edges_) {
        uint32_t new_u = vertexMapping_.at(edge.u);
        uint32_t new_v = vertexMapping_.at(edge.v);

        adjacency[new_u].emplace_back(new_v, edge.weight);
        if (new_u != new_v) { // Учет петель (не дублируем)
            adjacency[new_v].emplace_back(new_u, edge.weight);
        }
    }

    // Сортировка списка смежностей
    for (auto& list : adjacency) {
        std::sort(list.begin(), list.end());
    }

    // Запись списков смежности с дельта-кодированием
    for (uint32_t i = 0; i < vertexCount; ++i) {
        const auto& neighbors = adjacency[i];

        // Кол-во соседей
        uint32_t neighborCount = neighbors.size();
        result.insert(result.end(), reinterpret_cast<const uint8_t*>(&neighborCount),
                     reinterpret_cast<const uint8_t*>(&neighborCount) + 4);

        uint32_t prev_vertex = i; // Для дельта-кодирования
        for (const auto& [neighbor, weight] : neighbors) {
            // Дельта-кодирование: хранение разницы
            uint32_t delta = neighbor - prev_vertex;
            prev_vertex = neighbor;

            // Использование переменной длины для дельт
            if (delta < 128) {
                result.push_back(static_cast<uint8_t>(delta));
            } else if (delta < 16384) {
                result.push_back(static_cast<uint8_t>((delta >> 8) | 0x80));
                result.push_back(static_cast<uint8_t>(delta & 0xFF));
            } else {
                result.push_back(0xC0);
                result.insert(result.end(), reinterpret_cast<const uint8_t*>(&delta),
                             reinterpret_cast<const uint8_t*>(&delta) + 4);
            }

            // Вес
            result.push_back(weight);
        }
    }

    return result;
}
