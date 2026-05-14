#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>


CommandResult DataBase::TTL(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return std::unexpected("(error) wrong number of arguments for 'TTL'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "-2";
    }

    if (!it->second.ttl.has_value()) {
        return "-1";
    }

    auto now = std::chrono::steady_clock::now();
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
            it->second.ttl.value() - now).count();
    
    if (remaining < 0)
        return "-2";

    return std::to_string(remaining);
}