#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SMembers(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return std::unexpected("(error) wrong number of arguments for 'SMEMBERS'");
    }
    
    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "";
    }
    if (it->second.type != ValueType::kSet) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    SetType& set = std::get<SetType>(it->second.data);

    std::string result = "";

    for (auto set_it = set.begin(); set_it != set.end(); ++set_it) {
        result += *set_it + " ";
    }
    if (!result.empty()) {
        result.pop_back();
    }

    return result;
}