#pragma once
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

struct GeoPoint {
    double lon;
    double lat;
};

using StringType = std::string;
using ListType = std::list<std::string>;
using SetType = std::unordered_set<std::string>;
using GeoType = std::unordered_map<std::string, GeoPoint>;
using ValueData = std::variant<StringType, ListType, SetType, GeoType>;
using TimePoint = std::chrono::steady_clock::time_point;

using CommandResult = std::expected<std::string, std::string>;
using CommandHandler = std::function<CommandResult(const std::vector<std::string>&)>;

enum class ValueType {
    kString,
    kList,
    kSet,
    kGeoPos
};

struct Value {
    size_t memory_usage;
    ValueData data;
    ValueType type;
    std::optional<TimePoint> ttl;
};

class DataBase {
public:
    CommandResult Execute(const std::string& line);

    DataBase();
    ~DataBase() = default;

private:
    size_t memory_usage_;
    std::optional<size_t> memory_limit_;
    std::unordered_map<std::string, Value> db_;
    std::unordered_map<std::string, CommandHandler> handlers_;

    void ProccessExpiration(const std::string& key);
    double Haversine(double lon1, double lat1, double lon2, double lat2);
    size_t ParseMemory(const std::string& input);

    CommandResult Set(const std::vector<std::string>& args);
    CommandResult Get(const std::vector<std::string>& args);
    CommandResult Strlen(const std::vector<std::string>& args);
    CommandResult Append(const std::vector<std::string>& args);
    CommandResult Expire(const std::vector<std::string>& args);
    CommandResult TTL(const std::vector<std::string>& args);

    CommandResult LPush(const std::vector<std::string>& args);
    CommandResult RPush(const std::vector<std::string>& args);
    CommandResult LPop(const std::vector<std::string>& args);
    CommandResult RPop(const std::vector<std::string>& args);
    CommandResult LLen(const std::vector<std::string>& args);
    CommandResult LRange(const std::vector<std::string>& args);
    CommandResult LIndex(const std::vector<std::string>& args);
    CommandResult LSet(const std::vector<std::string>& args);
    CommandResult LInsert(const std::vector<std::string>& args);

    CommandResult SAdd(const std::vector<std::string>& args);
    CommandResult SRem(const std::vector<std::string>& args);
    CommandResult SIsMember(const std::vector<std::string>& args);
    CommandResult SMembers(const std::vector<std::string>& args);
    CommandResult SCard(const std::vector<std::string>& args);
    CommandResult SUnion(const std::vector<std::string>& args);
    CommandResult SInter(const std::vector<std::string>& args);
    CommandResult SDiff(const std::vector<std::string>& args);
    CommandResult SMove(const std::vector<std::string>& args);

    CommandResult GeoAdd(const std::vector<std::string>& args);
    CommandResult GeoPos(const std::vector<std::string>& args);
    CommandResult GeoDist(const std::vector<std::string>& args);
    CommandResult GeoSearch(const std::vector<std::string>& args);
    CommandResult GeoSearchStore(const std::vector<std::string>& args);

    CommandResult Type(const std::vector<std::string>& args);
    CommandResult Del(const std::vector<std::string>& args);
    CommandResult Exists(const std::vector<std::string>& args);
    CommandResult Keys(const std::vector<std::string>& args);
    CommandResult FlushDB(const std::vector<std::string>& args);
    CommandResult ConfigSet(const std::vector<std::string>& args);
    CommandResult ConfigGet(const std::vector<std::string>& args);
    CommandResult DBSize(const std::vector<std::string>& args);
    CommandResult MemoryUsage(const std::vector<std::string>& args);
};