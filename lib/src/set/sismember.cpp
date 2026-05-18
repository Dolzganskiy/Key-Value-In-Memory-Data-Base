#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SIsMember(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'SISMEMBER'");
    }
    
    const std::string& key = args[1];
    const std::string& value = args[2];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "0";
    }
    if (it->second.type != ValueType::kSet) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    SetType& set = std::get<SetType>(it->second.data);

    return set.find(value) != set.end() ? "1" : "0";
}