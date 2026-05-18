#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::Exists(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return std::unexpected("(error) wrong number of arguments for 'EXISTS'");
    }

    int count = 0;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& key = args[i];

        ProccessExpiration(key);

        if (db_.find(key) != db_.end()) {
            ++count;
        }
    }

    return std::to_string(count);
}