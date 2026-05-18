#include "DataBase.h"
#include <string>
#include <optional>
#include <chrono>

void DataBase::ProccessExpiration(const std::string& key) {
    auto it = db_.find(key);
    if (it == db_.end()) return;

    if (!it->second.ttl.has_value()) return;

    auto now = std::chrono::steady_clock::now();
    if (now <= it->second.ttl.value()) return;

    memory_usage_ -= it->second.memory_usage;
    db_.erase(it);
}