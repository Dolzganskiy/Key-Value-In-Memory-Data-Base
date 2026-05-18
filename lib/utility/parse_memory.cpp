#include "DataBase.h"

size_t ParseMemory(const std::string& input) {
    std::string value = input;
    std::string suffix;

    for (size_t i = 0; i < input.size(); ++i) {
        if (!std::isdigit(input[i])) {
            value = input.substr(0, i);
            suffix = input.substr(i);
            break;
        }
    }

    size_t number = std::stoull(value);

    if (suffix == "" || suffix == "b")
        return number;
    else if (suffix == "kb")
        return number * 1024;
    else if (suffix == "mb")
        return number * 1024 * 1024;
    else if (suffix == "gb")
        return number * 1024 * 1024 * 1024;
    else
        throw std::runtime_error("invalid memory suffix");
}