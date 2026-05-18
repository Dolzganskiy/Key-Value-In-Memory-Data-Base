#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::ConfigSet(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        return std::unexpected("(error) wrong number of arguments for 'CONFIG SET'");
    }

    if (args[2] != "maxmemory") {
        return std::unexpected("(error) unsupported CONFIG parameter");
    }

    size_t new_limit;

    try {
        new_limit = ParseMemory(args[3]);
    } catch (...) {
        return std::unexpected("(error) invalid memory value");
    }

    memory_limit_ = new_limit;

    return "OK";
}