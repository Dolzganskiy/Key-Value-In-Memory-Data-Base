#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::ConfigGet(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'CONFIG GET'");
    }

    if (args[2] != "maxmemory") {
        return std::unexpected("(error) unsupported CONFIG parameter");
    }

    size_t value = memory_limit_.has_value()
        ? memory_limit_.value()
        : 0;

    return "maxmemory " + std::to_string(value);
}