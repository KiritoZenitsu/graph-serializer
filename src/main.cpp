#include "graph.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iterator>

void printUsage() {
    std::cout << "Usage:\n"
              << "  Serialize: ./run -s -i input.tsv -o graph.bin\n"
              << "  Deserialize: ./run -d -i graph.bin -o output.tsv\n";
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printUsage();
        return 1;
    }

    std::string mode = argv[1];
    std::string input_file = argv[3];
    std::string output_file = argv[5];

    // Проверка флагов
    std::string input_flag = argv[2];
    std::string output_flag = argv[4];

    if (input_flag != "-i" || output_flag != "-o") {
        std::cerr << "Error: Expected -i and -o flags\n";
        printUsage();
        return 1;
    }

    Graph graph;

    if (mode == "-s") {
        // Сериализация
        if (!graph.readFromTSV(input_file)) {
            std::cerr << "Failed to read input TSV file\n";
            return 1;
        }

        auto binary_data = graph.serialize();

        std::ofstream out(output_file, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "Cannot create output file: " << output_file << std::endl;
            return 1;
        }

        out.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        std::cout << "Serialized " << graph.getEdges().size() << " edges to " << output_file << std::endl;

    } else if (mode == "-d") {
        // Десериализация
        std::ifstream in(input_file, std::ios::binary);
        if (!in.is_open()) {
            std::cerr << "Cannot open input file: " << input_file << std::endl;
            return 1;
        }

        std::vector<uint8_t> binary_data((std::istreambuf_iterator<char>(in)),
                                        std::istreambuf_iterator<char>());

        if (!graph.deserialize(binary_data)) {
            std::cerr << "Failed to deserialize binary file\n";
            return 1;
        }

        if (!graph.writeToTSV(output_file)) {
            std::cerr << "Failed to write output TSV file\n";
            return 1;
        }

        std::cout << "Deserialized " << graph.getEdges().size() << " edges to " << output_file << std::endl;

    } else {
        printUsage();
        return 1;
    }

    return 0;
}
