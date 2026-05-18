#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::Del(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return std::unexpected("(error) wrong number of arguments for 'DEL'");
    }

    int deleted = 0;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& key = args[i];

        ProccessExpiration(key);

        auto it = db_.find(key);
        if (it == db_.end()) {
            continue;
        }

        memory_usage_ -= it->second.memory_usage;
        db_.erase(it);
        ++deleted;
    }

    return std::to_string(deleted);
}