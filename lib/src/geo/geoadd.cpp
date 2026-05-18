#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::GeoAdd(const std::vector<std::string>& args) {
    if (args.size() < 5 || (args.size() - 2) % 3 != 0) {
        return std::unexpected("(error) wrong number of arguments for 'GEOADD'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);

    GeoType geo;
    bool created = false;

    if (it == db_.end()) {
        created = true;
    } else {
        if (it->second.type != ValueType::kGeoPos) {
            return std::unexpected(
                "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
            );
        }
        geo = std::get<GeoType>(it->second.data);
    }

    size_t delta = created ? key.size() : 0;
    int added = 0;

    for (size_t i = 2; i < args.size(); i += 3) {
        double lon, lat;
        try {
            lon = std::stod(args[i]);
            lat = std::stod(args[i + 1]);
        } catch (...) {
            return std::unexpected("(error) invalid float");
        }

        if (lon < -180 || lon > 180 ||
            lat < -90  || lat > 90) {
            return std::unexpected("(error) invalid longitude/latitude");
        }

        const std::string& member = args[i + 2];

        if (geo.find(member) == geo.end()) {
            delta += member.size();
            ++added;
        }

        geo[member] = {lon, lat};
    }

    if (memory_limit_.has_value() &&
        memory_usage_ + delta > memory_limit_.value()) {
        return std::unexpected(
            "(error) OOM command not allowed when used memory > 'maxmemory'"
        );
    }

    if (created) {
        Value val{delta, geo, ValueType::kGeoPos, std::nullopt};
        db_[key] = std::move(val);
    } else {
        it->second.memory_usage += delta;
        std::get<GeoType>(it->second.data) = std::move(geo);
    }

    memory_usage_ += delta;

    return std::to_string(added);
}