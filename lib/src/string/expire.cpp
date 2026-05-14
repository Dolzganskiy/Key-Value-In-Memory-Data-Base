#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <unordered_map>
#include <optional>
#include <chrono>

CommandResult DataBase::Expire(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        return std::unexpected("(error) wrong number of arguments for 'SET'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "0";
    }

    int seconds;
    try {
        seconds = std::stoi(args[2]);
    } catch (...) {
        return std::unexpected("(error) value is not an integer or out of range");
    }

    if (seconds <= 0) {
        memory_usage_ -= it->second.memory_usage;
        db_.erase(it);
        return "1";
    }

    it->second.ttl = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);

    return "1";
}