#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SUnion(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return std::unexpected("(error) wrong number of arguments for 'SUNION'");
    }

    std::unordered_set<std::string> result;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& key = args[i];

        ProccessExpiration(key);

        auto it = db_.find(key);
        if (it == db_.end()) {
            continue;
        }

        if (it->second.type != ValueType::kSet) {
            return std::unexpected(
                "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
            );
        }

        const SetType& set = std::get<SetType>(it->second.data);

        result.insert(set.begin(), set.end());
    }

    std::string output;

    for (auto it = result.begin(); it != result.end(); ++it) {
        output += *it;
        if (std::next(it) != result.end())
            output += " ";
    }

    return output;
}