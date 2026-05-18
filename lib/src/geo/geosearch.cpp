#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>
#include <algorithm>

CommandResult DataBase::GeoSearch(const std::vector<std::string>& args) {
    if (args.size() < 8) {
        return std::unexpected("(error) wrong number of arguments for 'GEOSEARCH'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "";
    }

    if (it->second.type != ValueType::kGeoPos) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    double lon, lat, radius;
    try {
        lon = std::stod(args[3]);
        lat = std::stod(args[4]);
        radius = std::stod(args[6]);
    } catch (...) {
        return std::unexpected("(error) invalid number");
    }

    std::string unit = args[7];

    const GeoType& geo = std::get<GeoType>(it->second.data);

    struct Item {
        std::string name;
        double dist;
    };

    std::vector<Item> matches;

    for (const auto& [member, point] : geo) {
        double dist_km = Haversine(
            lon, lat,
            point.lon, point.lat
        );

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
            matches.push_back({member, dist});
        }
    }

    bool asc = true;
    size_t count_limit = matches.size();

    for (size_t i = 8; i < args.size(); ++i) {
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
                return asc ? a.dist < b.dist
                            : a.dist > b.dist;
            });

    if (count_limit < matches.size()) {
        matches.resize(count_limit);
    }

    std::string result;
    for (size_t i = 0; i < matches.size(); ++i) {
        result += matches[i].name;
        if (i + 1 < matches.size())
            result += " ";
    }

    return result;
}