#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::LIndex(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'LINDEX'");
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
        return "(nil)";
    }

    auto list_it = list.begin();
    std::advance(list_it, index);

    std::string result = *list_it;

    return result;
}