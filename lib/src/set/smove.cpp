#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::SMove(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        return std::unexpected("(error) wrong number of arguments for 'SMOVE'");
    }

    const std::string& source_key = args[1];
    const std::string& dest_key = args[2];
    const std::string& value = args[3];

    ProccessExpiration(source_key);
    ProccessExpiration(dest_key);

    auto source_it = db_.find(source_key);
    if (source_it == db_.end()) {
        return "0";
    }

    if (source_it->second.type != ValueType::kSet) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    SetType& source_set =
        std::get<SetType>(source_it->second.data);

    auto value_it = source_set.find(value);
    if (value_it == source_set.end()) {
        return "0";
    }

    auto dest_it = db_.find(dest_key);

    if (dest_it != db_.end() &&
        dest_it->second.type != ValueType::kSet) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    size_t delta_add = 0;
    size_t delta_remove = value.size();

    bool dest_created = false;

    if (dest_it == db_.end()) {
        delta_add = dest_key.size() + value.size();
        dest_created = true;
    } else {
        SetType& dest_set =
            std::get<SetType>(dest_it->second.data);

        if (dest_set.find(value) == dest_set.end()) {
            delta_add = value.size();
        } else {
            delta_add = 0;
        }
    }

    if (memory_limit_.has_value() &&
        memory_usage_ - delta_remove + delta_add >
        memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    source_set.erase(value_it);
    source_it->second.memory_usage -= delta_remove;
    memory_usage_ -= delta_remove;

    if (source_set.empty()) {
        memory_usage_ -= source_key.size();
        db_.erase(source_it);
    }

    if (dest_created) {
        SetType new_set;
        new_set.insert(value);

        Value val{delta_add, new_set,
                  ValueType::kSet, std::nullopt};

        db_[dest_key] = std::move(val);
        memory_usage_ += delta_add;
    } else {
        SetType& dest_set =
            std::get<SetType>(db_[dest_key].data);

        if (dest_set.insert(value).second) {
            db_[dest_key].memory_usage += value.size();
            memory_usage_ += value.size();
        }
    }

    return "1";
}