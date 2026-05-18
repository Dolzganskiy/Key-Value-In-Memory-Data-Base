#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::GeoPos(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        return std::unexpected("(error) wrong number of arguments for 'GEOPOS'");
    }

    const std::string& key = args[1];

    ProccessExpiration(key);

    auto it = db_.find(key);

    std::string result;

    if (it == db_.end()) {
        for (size_t i = 2; i < args.size(); ++i) {
            result += "(nil)";
            if (i + 1 < args.size())
                result += " ";
        }
        return result;
    }

    if (it->second.type != ValueType::kGeoPos) {
        return std::unexpected(
            "(error) WRONGTYPE Operation against a key holding the wrong kind of value"
        );
    }

    const GeoType& geo =
        std::get<GeoType>(it->second.data);

    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& member = args[i];

        auto pos_it = geo.find(member);
        if (pos_it == geo.end()) {
            result += "(nil)";
        } else {
            result += std::to_string(pos_it->second.lon) + " " +
                      std::to_string(pos_it->second.lat);
        }

        if (i + 1 < args.size())
            result += " ";
    }

    return result;
}