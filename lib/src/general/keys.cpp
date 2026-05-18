#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <regex>

std::string GlobToRegex(const std::string& pattern) {
    std::string regex;

    for (char c : pattern) {
        if (c == '*') {
            regex += ".*";
        } else if (c == '?') {
            regex += ".";
        } else if (std::isalnum(c) || c == ':' || c == '_') {
            regex += c;
        } else {
            regex += "\\" + std::string(1, c);
        }
    }

    return "^" + regex + "$";
}

CommandResult DataBase::Keys(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return std::unexpected("(error) wrong number of arguments for 'KEYS'");
    }

    const std::string& pattern = args[1];

    std::string regex_pattern = GlobToRegex(pattern);
    std::regex re(regex_pattern);

    std::vector<std::string> keys;

    for (const auto& [key, _] : db_) {
        ProccessExpiration(key);
        if (db_.find(key) != db_.end()) {
            keys.push_back(key);
        }
    }

    std::string result;

    for (const auto& key : keys) {
        if (std::regex_match(key, re)) {
            result += key + " ";
        }
    }

    if (!result.empty())
        result.pop_back();

    return result;
}