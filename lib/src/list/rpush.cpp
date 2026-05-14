#include "DataBase.h"

CommandResult DataBase::RPush(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        return std::unexpected("(error) wrong number of arguments for 'LPUSH'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);

    if (it == db_.end()) {
        size_t weight = key.size();
        ListType list;

        for (size_t i = 2; i < args.size(); ++i) {
            const std::string& value = args[i];
            weight += value.size();
        }

        if (memory_limit_.has_value() && 
            memory_usage_ + weight > memory_limit_.value()) {
            return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
        }

        for (size_t i = 2; i < args.size(); ++i) {
            const std::string& value = args[i];
            list.push_back(value);
        }

        Value val{weight, list, ValueType::kList, std::nullopt};
        db_[key] = std::move(val);
        memory_usage_ += weight;

        return std::to_string(list.size());
    }

    if (it->second.type != ValueType::kList) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    ListType& list = std::get<ListType>(it->second.data);
    
    size_t delta = 0;
    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& value = args[i];
        delta += value.size();
    }
    if (memory_limit_.has_value() && 
        memory_usage_ + delta > memory_limit_.value()) {
        return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
    }

    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& value = args[i];
        list.push_back(value);
    }

    it->second.memory_usage += delta;
    memory_usage_ += delta;

    return std::to_string(list.size()); 
}