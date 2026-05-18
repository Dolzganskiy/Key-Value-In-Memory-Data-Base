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

struct GeoPos {
    double lon;
    double lat;
};

using StringType = std::string;
using ListType = std::list<std::string>;
using SetType = std::unordered_set<std::string>;
using GeoType = std::unordered_map<std::string, GeoPos>;
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
};

/*
LPUSH	LPUSH key value [value ...]	Добавить элементы в начало списка
RPUSH	RPUSH key value [value ...]	Добавить элементы в конец списка
LPOP	LPOP key [count]	Извлечь и вернуть элементы из начала списка
RPOP	RPOP key [count]	Извлечь и вернуть элементы из конца списка
LLEN	LLEN key	Вернуть длину списка
LRANGE	LRANGE key start stop	Вернуть подсписок по индексам (поддерживаются отрицательные индексы)
LINDEX	LINDEX key index	Вернуть элемент по индексу
LSET	LSET key index value	Установить значение элемента по индексу
LINSERT	LINSERT key BEFORE|AFTER pivot value	Вставить элемент до или после опорного значения
*/

/*
SADD	SADD key member [member ...]	Добавить элементы в множество
SREM	SREM key member [member ...]	Удалить элементы из множества
SISMEMBER	SISMEMBER key member	Проверить принадлежность элемента множеству
SMEMBERS	SMEMBERS key	Вернуть все элементы множества
SCARD	SCARD key	Вернуть количество элементов
SUNION	SUNION key [key ...]	Объединение нескольких множеств
SINTER	SINTER key [key ...]	Пересечение нескольких множеств
SDIFF	SDIFF key [key ...]	Разность множеств (элементы первого, отсутствующие в остальных)
SMOVE	SMOVE source destination member	Переместить элемент из одного множества в другое
*/

/*
GEOADD	GEOADD key longitude latitude member [longitude latitude member ...]	Добавить точки с координатами
GEOPOS	GEOPOS key member [member ...]	Получить координаты точек
GEODIST	GEODIST key member1 member2 [unit]	Вычислить расстояние между двумя точками. Единицы: m, km, mi, ft
GEOSEARCH	GEOSEARCH key FROMLONLAT lon lat BYRADIUS radius unit [ASC|DESC] [COUNT count]	Найти точки в заданном радиусе от координаты
GEOSEARCHSTORE	GEOSEARCHSTORE dest source FROMLONLAT lon lat BYRADIUS radius unit [ASC|DESC] [COUNT count]	То же, но сохранить результат в новый ключ
*/