#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SDiff(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return std::unexpected("(error) wrong number of arguments for 'SDIFF'");
    }

    const std::string& first_key = args[1];

    ProccessExpiration(first_key);

    auto it = db_.find(first_key);
    if (it == db_.end()) {
        return "";
    }

    if (it->second.type != ValueType::kSet) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    std::unordered_set<std::string> result =
        std::get<SetType>(it->second.data);

    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& key = args[i];

        ProccessExpiration(key);

        auto other_it = db_.find(key);
        if (other_it == db_.end()) {
            continue;
        }

        if (other_it->second.type != ValueType::kSet) {
            return std::unexpected(
                "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
            );
        }

        const SetType& other_set =
            std::get<SetType>(other_it->second.data);

        for (auto res_it = result.begin(); res_it != result.end(); ) {
            if (other_set.find(*res_it) != other_set.end()) {
                res_it = result.erase(res_it);
            } else {
                ++res_it;
            }
        }

        if (result.empty()) {
            return "";
        }
    }

    std::string output;
    for (auto it = result.begin(); it != result.end(); ++it) {
        output += *it;
        if (std::next(it) != result.end())
            output += " ";
    }

    return output;
}