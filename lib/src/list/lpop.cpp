#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <algorithm>

CommandResult DataBase::LPop(const std::vector<std::string>& args) {
    if (args.size() != 2 && args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'LPOP'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "(nil)";
    }

    if (it->second.type != ValueType::kList) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    int count = 1;
    if (args.size() == 3) {
        try {
            count += std::stoi(args[2]);
        } catch(...) {
            return std::unexpected("(error) value is not an integer or out of range");
        }
        if (count <= 0) {
            return std::unexpected("(error) value is not an integer or out of range");
        }
    }

    ListType& list = std::get<ListType>(it->second.data);

    size_t real_count = std::min(static_cast<size_t>(count), list.size());

    if (real_count == 0) {
        return "(nil)";
    }

    std::string result;
    size_t delta = 0;

    for (size_t i = 0; i < real_count; ++i) {
        const std::string& value = list.front();
        delta += value.size();
        result += value + " ";
        memory_usage_ -= value.size();
        list.pop_front();
    }
    result.pop_back();

    it->second.memory_usage -= delta;
    memory_usage_ -= delta;

    if (list.empty()) {
        memory_usage_ -= key.size();
        db_.erase(key);
    }

    return result;
}