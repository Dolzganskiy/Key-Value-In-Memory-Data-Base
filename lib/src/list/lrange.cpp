#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::LRange(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        return std::unexpected("(error) wrong number of arguments for 'LRANGE'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "";
    }

    if (it->second.type != ValueType::kList) {
        return std::unexpected("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    int start;
    int stop;
    try {
        start = std::stoi(args[2]);
        stop = std::stoi(args[3]);
    } catch (...) {
        return std::unexpected("(error) value is not an integer or out of range");
    }
    
    ListType& list = std::get<ListType>(it->second.data);

    if (list.empty()) {
        return "";
    }

    int n = static_cast<int>(list.size());

    if (start < 0) {
        start += n;
    }
    if (stop < 0) {
        stop += n;
    }

    start = std::max(0, start);
    stop = std::min(n - 1, stop);

    if (start > stop) {
        return "";
    }

    auto list_it = list.begin();
    std::advance(list_it, start);

    std::string result = "";
    for (int i = start; i <= stop; ++i) {
        result += *list_it + " ";
        ++list_it;
    }
    if (!result.empty()) {
        result.pop_back();
    }

    return result;

}