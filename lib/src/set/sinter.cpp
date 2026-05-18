#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SInter(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return std::unexpected("(error) wrong number of arguments for 'SINTER'");
    }

    std::unordered_set<std::string> result;
    bool first = true;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& key = args[i];

        ProccessExpiration(key);

        auto it = db_.find(key);
        if (it == db_.end()) {
            return "";
        }

        if (it->second.type != ValueType::kSet) {
            return std::unexpected(
                "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
            );
        }

        const SetType& set = std::get<SetType>(it->second.data);

        if (first) {
            result = set;
            first = false;
        } else {
            for (auto res_it = result.begin(); res_it != result.end(); ) {
                if (set.find(*res_it) == set.end()) {
                    res_it = result.erase(res_it);
                } else {
                    ++res_it;
                }
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