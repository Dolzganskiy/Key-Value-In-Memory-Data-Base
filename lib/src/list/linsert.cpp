#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::LInsert(const std::vector<std::string>& args) {
    if (args.size() != 5) {
        return std::unexpected("(error) wrong number of arguments for 'LINSERT'");
    }

    const std::string& key = args[1];
    const std::string& where = args[2];
    const std::string& pivot = args[3];
    const std::string& value = args[4];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "0";
    }

    if (it->second.type != ValueType::kList) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    ListType& list = std::get<ListType>(it->second.data);

    auto pos = std::find(list.begin(), list.end(), pivot);
    if (pos == list.end()) {
        return "-1";
    }

    size_t delta = value.size();

    if (memory_limit_.has_value() &&
        memory_usage_ + delta > memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    if (where == "BEFORE") {
        list.insert(pos, value);
    } else if (where == "AFTER") {
        ++pos;
        list.insert(pos, value);
    } else {
        return std::unexpected(
            "(error) syntax error"
        );
    }

    it->second.memory_usage += delta;
    memory_usage_ += delta;

    return std::to_string(list.size());
}