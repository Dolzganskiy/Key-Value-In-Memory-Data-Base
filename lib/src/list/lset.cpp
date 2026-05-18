#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::LSet(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        return std::unexpected("(error) wrong number of arguments for 'LSET'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return std::unexpected("(error) no such key");
    }

    if (it->second.type != ValueType::kList) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    int index;
    try {
        index = std::stoi(args[2]);
    } catch (...) {
        return std::unexpected("(error) value is not an integer or out of range");
    }
    
    ListType& list = std::get<ListType>(it->second.data);

    int n = static_cast<int>(list.size());

    if (index < 0) {
        index += n;
    }

    if (index < 0 || index >= n) {
        return std::unexpected("(error) index out of range");
    }

    const std::string& value = args[3];
    
    auto list_it = list.begin();
    std::advance(list_it, index);
    
    const std::string& old_value = *list_it;
    int delta = static_cast<int>(value.size()) - static_cast<int>(old_value.size());

    if (memory_limit_.has_value() &&
        memory_usage_ + delta > memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    *list_it = value;

    memory_usage_ += delta;
    it->second.memory_usage += delta;
    
    return "OK";
}