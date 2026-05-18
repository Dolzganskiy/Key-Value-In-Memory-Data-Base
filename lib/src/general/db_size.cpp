#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::DBSize(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return std::unexpected("(error) wrong number of arguments for 'DBSIZE'");
    }

    std::vector<std::string> keys;
    for (const auto& [key, _] : db_) {
        keys.push_back(key);
    }

    for (const auto& key : keys) {
        ProccessExpiration(key);
    }

    return std::to_string(db_.size());
}