#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::MemoryUsage(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'MEMORY USAGE'");
    }

    const std::string& key = args[2];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "(nil)";
    }

    return std::to_string(it->second.memory_usage);
}