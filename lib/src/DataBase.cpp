#include "DataBase.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <unordered_set>
#include <variant>
#include <optional>
#include <chrono>
#include <functional>
#include <expected>
#include <list>

DataBase::DataBase() : memory_usage_(0), memory_limit_(std::nullopt) {
    
    handlers_["SET"] = [this](const auto& args) { return Set(args); };
    handlers_["GET"] = [this](const auto& args) { return Get(args); };
    handlers_["STRLEN"] = [this](const auto& args) { return Strlen(args); };
    handlers_["APPEND"] = [this](const auto& args) { return Append(args); };
    handlers_["EXPIRE"] = [this](const auto& args) { return Expire(args); };
    handlers_["TTL"] = [this](const auto& args) { return TTL(args); };

    handlers_["LPUSH"] = [this](const auto& args) { return LPush(args); };
    handlers_["RPUSH"] = [this](const auto& args) { return RPush(args); };
    handlers_["LPOP"] = [this](const auto& args) { return LPop(args); };
    handlers_["RPOP"] = [this](const auto& args) { return RPop(args); };
    handlers_["LLEN"] = [this](const auto& args) { return LLen(args); };
    handlers_["LRANGE"] = [this](const auto& args) { return LRange(args); };
    handlers_["LINDEX"] = [this](const auto& args) { return LIndex(args); };
    handlers_["LSET"] = [this](const auto& args) { return LSet(args); };
    handlers_["LINSERT"] = [this](const auto& args) { return LInsert(args); };

    handlers_["SADD"] = [this](const auto& args) { return SAdd(args); };
    handlers_["SREM"] = [this](const auto& args) { return SRem(args); };
    handlers_["SISMEMBER"] = [this](const auto& args) { return SIsMember(args); };
    handlers_["SMEMBERS"] = [this](const auto& args) { return SMembers(args); };
    handlers_["SCARD"] = [this](const auto& args) { return SCard(args); };
    handlers_["SUNION"] = [this](const auto& args) { return SUnion(args); };
    handlers_["SINTER"] = [this](const auto& args) { return SInter(args); };
    handlers_["SDIFF"] = [this](const auto& args) { return SDiff(args); };
    handlers_["SMOVE"] = [this](const auto& args) { return SMove(args); };

    handlers_["GEOADD"] = [this](const auto& args) { return GeoAdd(args); };
    handlers_["GEOPOS"] = [this](const auto& args) { return GeoPos(args); };
    handlers_["GEODIST"] = [this](const auto& args) { return GeoDist(args); };
    handlers_["GEOSEARCH"] = [this](const auto& args) { return GeoSearch(args); };
    handlers_["GEOSEARCHSTORE"] = [this](const auto& args) { return GeoSearchStore(args); };

    handlers_["TYPE"] = [this](const auto& args) { return Type(args); };
    handlers_["DEL"] = [this](const auto& args) { return Del(args); };
    handlers_["EXISTS"] = [this](const auto& args) { return Exists(args); };
    handlers_["KEYS"] = [this](const auto& args) { return Keys(args); };
    handlers_["FLUSHDB"] = [this](const auto& args) { return FlushDB(args); };
    handlers_["CONFIG"] = [this](const auto& args) -> CommandResult { 
        if (args.size() >= 2) {
            std::string sub = args[1];
            std::transform(sub.begin(), sub.end(), sub.begin(), 
                [](unsigned char c) {return std::toupper(c); });

            if (sub == "SET") return ConfigSet(args);
            if (sub == "GET") return ConfigGet(args);
        }
        return std::unexpected("(error) wrong CONFIG command"); };
    handlers_["DBSIZE"] = [this](const auto& args) { return DBSize(args); };
    handlers_["MEMORY"] = [this](const auto& args) -> CommandResult { 
        if (args.size() >= 2) {
            std::string sub = args[1];
            std::transform(sub.begin(), sub.end(), sub.begin(), 
                [](unsigned char c) {return std::toupper(c); });

            if (sub == "USAGE") return MemoryUsage(args);
        }
        return std::unexpected("(error) wrong MEMORY command"); };
}