#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SRem(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        return std::unexpected("(error) wrong number of arguments for 'SREM'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "0";
    }

    if (it->second.type != ValueType::kSet) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    SetType& set = std::get<SetType>(it->second.data);
    
    size_t delta = 0;
    int deleted = 0;

    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& value = args[i];
        if (set.find(value) != set.end()) {
            delta += value.size();
            set.erase(value);
            ++deleted;
        }
    }

    it->second.memory_usage -= delta;
    memory_usage_ -= delta;

    if (set.empty()) {
        memory_usage_ -= key.size();
        db_.erase(key);
    }

    return std::to_string(deleted); 
}