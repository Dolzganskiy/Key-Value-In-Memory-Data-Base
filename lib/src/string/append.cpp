#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <optional>

CommandResult DataBase::Append(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'APPEND'");
    }

    const std::string& key = args[1];
    const std::string& append_value = args[2];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        size_t new_weight = key.size() + append_value.size();
        if (memory_limit_.has_value() && 
            memory_usage_ + new_weight > memory_limit_.value()) {
            return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
        }

        Value val{new_weight, append_value, ValueType::kString, std::nullopt};
        db_[key] = val;
        memory_usage_ += new_weight;

        return "OK";
    }

    if (it->second.type != ValueType::kString) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    std::string& str = std::get<std::string>(it->second.data);

    size_t delta = append_value.size();

    if (memory_limit_.has_value() &&
        memory_usage_ + delta > memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    str += append_value;
    it->second.memory_usage += delta;
    memory_usage_ += delta;

    return "OK";
}