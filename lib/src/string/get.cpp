#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>

CommandResult DataBase::Get(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return std::unexpected("(error) wrong number of arguments for 'SET'");
    }
    
    const std::string& key = args[1];

    auto it = db_.find(key);
    if (it == db_.end()) {
        return std::unexpected("(nil)");
    }
    if (it->second.type != ValueType::kString) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    return std::get<std::string>(it->second.data);


}

