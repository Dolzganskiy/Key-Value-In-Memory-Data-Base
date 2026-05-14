#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <optional>

CommandResult DataBase::Set(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'SET'");
    }

    const std::string& key = args[1];
    const std::string& value = args[2];
    
    size_t new_weight = value.size() + key.size();
    size_t old_weight = 0;

    auto it = db_.find(key);
    if (it != db_.end()) {
        old_weight = it->second.memory_usage;
    }

    if (memory_limit_.has_value() && 
        memory_usage_ - old_weight + new_weight > memory_limit_.value()) {
        return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
    }

    memory_usage_ = memory_usage_ - old_weight + new_weight;
    Value val {new_weight, value, ValueType::kString, std::nullopt};
    db_[key] = std::move(val);

    return "OK";
}