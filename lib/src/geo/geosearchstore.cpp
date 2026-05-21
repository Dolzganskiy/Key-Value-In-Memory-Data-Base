#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <algorithm>

struct Item {
    std::string name;
    GeoPoint point;
    double dist;
};

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
            return std::unexpected(
                "(error) OOM command not allowed when used memory > 'maxmemory'"
            );
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

    const GeoType& source_geo = std::get<GeoType>(src_it->second.data);

    double lon, lat, radius;
    std::string unit;

    try {
        lon = std::stod(args[4]);
        lat = std::stod(args[5]);
        radius = std::stod(args[7]);
        unit = args[8];
    } catch (...) {
        return std::unexpected("(error) invalid number");
    }

    std::vector<Item> matches;

    for (const auto& [member, point] : source_geo) {
        double dist_km = Haversine(lon, lat, point.lon, point.lat);

        double dist;
        if (unit == "km")
            dist = dist_km;
        else if (unit == "m")
            dist = dist_km * 1000;
        else if (unit == "mi")
            dist = dist_km * 0.621371;
        else if (unit == "ft")
            dist = dist_km * 3280.84;
        else
            return std::unexpected("(error) unsupported unit");

        if (dist <= radius) {
            matches.push_back({member, point, dist});
        }
    }

    bool asc = true;
    size_t count_limit = matches.size();

    for (size_t i = 9; i < args.size(); ++i) {
        if (args[i] == "DESC")
            asc = false;
        else if (args[i] == "ASC")
            asc = true;
        else if (args[i] == "COUNT" && i + 1 < args.size()) {
            count_limit = std::stoul(args[i + 1]);
        }
    }

    std::sort(matches.begin(), matches.end(),
              [asc](const Item& a, const Item& b) {
                  return asc ? a.dist < b.dist : a.dist > b.dist;
              });

    if (count_limit < matches.size()) {
        matches.resize(count_limit);
    }

    GeoType new_geo;
    size_t new_weight = dest_key.size();

    for (const auto& item : matches) {
        new_geo[item.name] = item.point;
        new_weight += item.name.size();
    }

    size_t old_weight = 0;
    auto dest_it = db_.find(dest_key);
    if (dest_it != db_.end()) {
        old_weight = dest_it->second.memory_usage;
    }

    if (memory_limit_.has_value() &&
        memory_usage_ - old_weight + new_weight > memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    memory_usage_ = memory_usage_ - old_weight + new_weight;

    Value val{new_weight, new_geo, ValueType::kGeoPos, std::nullopt};
    db_[dest_key] = std::move(val);

    return std::to_string(matches.size());
}