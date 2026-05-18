#include <iostream>
#include <string>
#include "DataBase.h"

int main(int argc, const char** argv) {
    DataBase db;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--maxmemory" && i + 1 < argc) {
            try {
                size_t limit = db.ParseMemory(argv[i + 1]);
                db.SetMemoryLimit(limit);
                ++i;
            } catch(...) {
                std::cerr << "(error) invalid maxmemory value" << std::endl;
                return 1;
            }
        }
    }

    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        if (line == "EXIT") {
            break;
        }

        try {
            auto result = db.Execute(line);

            if (result.has_value()) {
                std::cout << result.value() << std::endl;
            } else {
                std::cerr << result.error() << std::endl;
            }
        } catch(const std::exception& e) {
            std::cerr << "(fatal) " << e.what() << std::endl;
        }
    }
}