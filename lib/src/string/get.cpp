#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::Get(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return std::unexpected("(error) wrong number of arguments for 'GET'");
    }
    
    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "(nil)";
    }
    if (it->second.type != ValueType::kString) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    return std::get<StringType>(it->second.data);
}

// unknown command	(error) unknown command
// wrong arg count	(error) wrong number of arguments for 'CMD'
// wrong type	(error) WRONGTYPE Operation against a key holding the wrong kind of value
// OOM	(error) OOM command not allowed when used memory > 'maxmemory'
// GET нет ключа	(nil)
// EXISTS нет ключа	0
// TTL нет ключа	-2