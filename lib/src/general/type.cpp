#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::Type(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return std::unexpected("(error) wrong number of arguments for 'TYPE'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "none";
    }

    switch (it->second.type) {
        case ValueType::kString:
            return "string";
        case ValueType::kList:
            return "list";
        case ValueType::kSet:
            return "set";
        case ValueType::kGeoPos:
            return "zset";
        default:
            return "none";
    }
}