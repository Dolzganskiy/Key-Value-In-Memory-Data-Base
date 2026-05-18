#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <algorithm>

CommandResult DataBase::GeoSearchStore(const std::vector<std::string>& args) {
    if (args.size() < 9) {
        return std::unexpected("(error) wrong number of arguments for 'GEOSEARCHSTORE'");
    }
    const std::string& dest_key = args[1];
    const std::string& source_key = args[2];

    ProccessExpiration(source_key);
    ProccessExpiration(dest_key);

    auto src_it = db_.find(source_key);
    if (src_it == db_.end()) {
        size_t delta = dest_key.size();

        if (memory_limit_.has_value() &&
            memory_usage_ + delta > memory_limit_.value()) {
            return std::unexpected("(error) OOM command not allowed when used memory > 'maxmemory'");
        }

        GeoType empty;
        Value val{delta, empty, ValueType::kGeoPos, std::nullopt};

        db_[dest_key] = std::move(val);
        memory_usage_ += delta;

        return "0";
    }

    if (src_it->second.type != ValueType::kGeoPos) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    const GeoType& source_geo =
        std::get<GeoType>(src_it->second.data);

    std::vector<std::pair<std::string, GeoPoint>> matches;

    GeoType new_geo;
    size_t new_weight = dest_key.size();

    for (auto& [member, point] : matches) {
        new_geo[member] = point;
        new_weight += member.size();
    }

    size_t old_weight = 0;
    auto dest_it = db_.find(dest_key);
    if (dest_it != db_.end()) {
        old_weight = dest_it->second.memory_usage;
    }

    if (memory_limit_.has_value() &&
        memory_usage_ - old_weight + new_weight >
        memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    memory_usage_ = memory_usage_ - old_weight + new_weight;

    Value val{new_weight, new_geo, ValueType::kGeoPos, std::nullopt};
    db_[dest_key] = std::move(val);

    return std::to_string(matches.size());
}