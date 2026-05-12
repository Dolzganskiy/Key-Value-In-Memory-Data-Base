#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <unordered_set>
#include <variant>
#include <optional>
#include <chrono>
#include <functional>

struct GeoPos {
    double lon;
    double lat;
};

using StringType = std::string;
using ListType = std::vector<std::string>;
using SetType = std::unordered_set<std::string>;
using GeoType = std::unordered_map<std::string, GeoPos>;

using ValueData = std::variant<StringType, ListType, SetType, GeoType>;

using TimePoint = std::chrono::steady_clock::time_point;

enum class ValueType {
    kString,
    kList,
    kSet,
    kGeoPos
};

struct Value {
    ValueType type;
    ValueData data;
    std::optional<TimePoint> ttl;
};

class DataBase {
private:
    using CommandHandler = std::function<std::string(const std::vector<std::string>&)>;

    std::unordered_map<std::string, Value> db_;
    std::unordered_map<std::string, CommandHandler> handlers;
public:
    std::string Execute(const std::string& line);
private:
    std::string Set(const std::vector<std::string>& args);
    std::string Get(const std::vector<std::string>& args);
    std::string Strlen(const std::vector<std::string>& args);
    std::string Append(const std::vector<std::string>& args);
    std::string Expire(const std::vector<std::string>& args);
    std::string TTL(const std::vector<std::string>& args);

    std::string LPush(const std::vector<std::string>& args);
    std::string RPush(const std::vector<std::string>& args);
    std::string LPop(const std::vector<std::string>& args);
    std::string RPop(const std::vector<std::string>& args);
    std::string LLen(const std::vector<std::string>& args);
    std::string LRange(const std::vector<std::string>& args);
    std::string LIndex(const std::vector<std::string>& args);
    std::string LSet(const std::vector<std::string>& args);
    std::string LInsert(const std::vector<std::string>& args);

    std::string SAdd(const std::vector<std::string>& args);
    std::string SRem(const std::vector<std::string>& args);
    std::string SIsMember(const std::vector<std::string>& args);
    std::string SMembers(const std::vector<std::string>& args);
    std::string SCard(const std::vector<std::string>& args);
    std::string SUnion(const std::vector<std::string>& args);
    std::string SInter(const std::vector<std::string>& args);
    std::string SDiff(const std::vector<std::string>& args);
    std::string SMove(const std::vector<std::string>& args);

    std::string GeoAdd(const std::vector<std::string>& args);
    std::string GeoPos(const std::vector<std::string>& args);
    std::string GeoDist(const std::vector<std::string>& args);
    std::string GeoSearch(const std::vector<std::string>& args);
    std::string GeoSearchStore(const std::vector<std::string>& args);
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