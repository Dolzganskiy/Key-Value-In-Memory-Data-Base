#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::GeoDist(const std::vector<std::string>& args) {
    if (args.size() != 4 && args.size() != 5) {
        return std::unexpected("(error) wrong number of arguments for 'GEODIST'");
    }

    const std::string& key = args[1];
    const std::string& member1 = args[2];
    const std::string& member2 = args[3];

    std::string unit = "m";
    if (args.size() == 5) {
        unit = args[4];
    }

    ProccessExpiration(key);

    auto it = db_.find(key);
    if (it == db_.end()) {
        return "(nil)";
    }

    if (it->second.type != ValueType::kGeoPos) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    const GeoType& geo = std::get<GeoType>(it->second.data);

    auto it1 = geo.find(member1);
    auto it2 = geo.find(member2);

    if (it1 == geo.end() || it2 == geo.end()) {
        return "(nil)";
    }

    double dist_km = Haversine(
        it1->second.lon, it1->second.lat,
        it2->second.lon, it2->second.lat
    );

    double result;

    if (unit == "km") {
        result = dist_km;
    } else if (unit == "m") {
        result = dist_km * 1000;
    } else if (unit == "mi") {
        result = dist_km * 0.621371;
    } else if (unit == "ft") {
        result = dist_km * 3280.84;
    } else {
        return std::unexpected("(error) unsupported unit");
    }

    return std::to_string(result);
}