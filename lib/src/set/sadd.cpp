#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SAdd(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        return std::unexpected("(error) wrong number of arguments for 'SADD'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        size_t weight = key.size();
        SetType set;
        int added = 0;

        for (size_t i = 2; i < args.size(); ++i) {
            const std::string& value = args[i];
            if (set.insert(value).second) {
                weight += value.size();
                ++added;
            }
        }

        if (memory_limit_.has_value() && 
            memory_usage_ + weight > memory_limit_.value()) {
            return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
        }

        Value val{weight, set, ValueType::kSet, std::nullopt};
        db_[key] = std::move(val);
        memory_usage_ += weight;

        return std::to_string(added);
    }

    if (it->second.type != ValueType::kSet) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    SetType& set = std::get<SetType>(it->second.data);
    
    size_t delta = 0;
    int added = 0;

    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& value = args[i];
        if (set.find(value) == set.end()) {
            delta += value.size();
            ++added;
        }
    }

    if (memory_limit_.has_value() && 
        memory_usage_ + delta > memory_limit_.value()) {
        return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
    }

    for (size_t i = 2; i < args.size(); ++i) {
        set.insert(args[i]);
    }

    it->second.memory_usage += delta;
    memory_usage_ += delta;

    return std::to_string(added); 
}