#include <gtest/gtest.h>
#include "DataBase.h"

class DBTest : public ::testing::Test {
protected:
    DataBase db;

    std::string exec(const std::string& cmd) {
        auto res = db.Execute(cmd);
        if (res.has_value())
            return res.value();
        return res.error();
    }
};

TEST_F(DBTest, SetAndGet) {
    EXPECT_EQ(exec("SET a 1"), "OK");
    EXPECT_EQ(exec("GET a"), "1");
}

TEST_F(DBTest, GetNonExisting) {
    EXPECT_EQ(exec("GET x"), "(nil)");
}

TEST_F(DBTest, Strlen) {
    exec("SET a hello");
    EXPECT_EQ(exec("STRLEN a"), "5");
}

TEST_F(DBTest, AppendCreatesKey) {
    EXPECT_EQ(exec("APPEND a hi"), "2");
}

TEST_F(DBTest, AppendWorks) {
    exec("SET a hi");
    EXPECT_EQ(exec("APPEND a there"), "7");
}

TEST_F(DBTest, TypeString) {
    exec("SET a 1");
    EXPECT_EQ(exec("TYPE a"), "string");
}

TEST_F(DBTest, OverwriteType) {
    exec("SET a 1");
    exec("LPUSH a x");
    EXPECT_EQ(exec("TYPE a"), "list");
}

TEST_F(DBTest, SetResetsTTL) {
    exec("SET a 1");
    exec("EXPIRE a 100");
    exec("SET a 2");
    EXPECT_EQ(exec("TTL a"), "-1");
}

TEST_F(DBTest, WrongTypeGet) {
    exec("LPUSH a 1");
    EXPECT_TRUE(exec("GET a").find("WRONGTYPE") != std::string::npos);
}

TEST_F(DBTest, UnknownCommand) {
    EXPECT_TRUE(exec("FOO").find("unknown") != std::string::npos);
}

TEST_F(DBTest, LPushAndLLen) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LLEN a"), "3");
}

TEST_F(DBTest, LPopSingle) {
    exec("LPUSH a 1 2");
    EXPECT_EQ(exec("LPOP a"), "2");
}

TEST_F(DBTest, LPopCount) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LPOP a 2"), "3 2");
}

TEST_F(DBTest, LIndexPositive) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LINDEX a 0"), "3");
}

TEST_F(DBTest, LIndexNegative) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LINDEX a -1"), "1");
}

TEST_F(DBTest, LIndexOutOfRange) {
    exec("LPUSH a 1");
    EXPECT_EQ(exec("LINDEX a 10"), "(nil)");
}

TEST_F(DBTest, LSetWorks) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LSET a 0 X"), "OK");
}

TEST_F(DBTest, LSetOutOfRange) {
    exec("LPUSH a 1");
    EXPECT_TRUE(exec("LSET a 10 X").find("range") != std::string::npos);
}

TEST_F(DBTest, LInsertBefore) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LINSERT a BEFORE 2 X"), "4");
}

TEST_F(DBTest, LInsertPivotMissing) {
    exec("LPUSH a 1");
    EXPECT_EQ(exec("LINSERT a BEFORE Z X"), "-1");
}

TEST_F(DBTest, LRangeBasic) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LRANGE a 0 1"), "3 2");
}

TEST_F(DBTest, LRangeNegative) {
    exec("LPUSH a 1 2 3");
    EXPECT_EQ(exec("LRANGE a -2 -1"), "2 1");
}

TEST_F(DBTest, LRangeEmpty) {
    EXPECT_EQ(exec("LRANGE a 0 1"), "");
}

TEST_F(DBTest, LPopEmptyListRemovesKey) {
    exec("LPUSH a 1");
    exec("LPOP a");
    EXPECT_EQ(exec("EXISTS a"), "0");
}

TEST_F(DBTest, SAddWorks) {
    EXPECT_EQ(exec("SADD s a b"), "2");
}

TEST_F(DBTest, SAddDuplicates) {
    exec("SADD s a");
    EXPECT_EQ(exec("SADD s a"), "0");
}

TEST_F(DBTest, SCard) {
    exec("SADD s a b");
    EXPECT_EQ(exec("SCARD s"), "2");
}

TEST_F(DBTest, SIsMemberTrue) {
    exec("SADD s a");
    EXPECT_EQ(exec("SISMEMBER s a"), "1");
}

TEST_F(DBTest, SIsMemberFalse) {
    EXPECT_EQ(exec("SISMEMBER s a"), "0");
}

TEST_F(DBTest, SRemWorks) {
    exec("SADD s a b");
    EXPECT_EQ(exec("SREM s a"), "1");
}

TEST_F(DBTest, SUnion) {
    exec("SADD s1 a b");
    exec("SADD s2 b c");
    EXPECT_FALSE(exec("SUNION s1 s2").empty());
}

TEST_F(DBTest, SInter) {
    exec("SADD s1 a b");
    exec("SADD s2 b c");
    EXPECT_EQ(exec("SINTER s1 s2"), "b");
}

TEST_F(DBTest, SDiff) {
    exec("SADD s1 a b");
    exec("SADD s2 b");
    EXPECT_EQ(exec("SDIFF s1 s2"), "a");
}

TEST_F(DBTest, SMove) {
    exec("SADD s1 a");
    EXPECT_EQ(exec("SMOVE s1 s2 a"), "1");
}

TEST_F(DBTest, GeoAdd) {
    EXPECT_EQ(exec("GEOADD g 10 10 A"), "1");
}

TEST_F(DBTest, GeoPos) {
    exec("GEOADD g 10 10 A");
    EXPECT_FALSE(exec("GEOPOS g A").empty());
}

TEST_F(DBTest, GeoDist) {
    exec("GEOADD g 10 10 A 20 20 B");
    EXPECT_FALSE(exec("GEODIST g A B km").empty());
}

TEST_F(DBTest, GeoSearch) {
    exec("GEOADD g 10 10 A 11 11 B");
    EXPECT_FALSE(exec("GEOSEARCH g FROMONLAT 10 10 BYRADIUS 200 km").empty());
}

TEST_F(DBTest, GeoSearchStore) {
    exec("GEOADD g 10 10 A");
    EXPECT_EQ(exec("GEOSEARCHSTORE g2 g FROMONLAT 10 10 BYRADIUS 100 km"), "1");
}

TEST_F(DBTest, Exists) {
    exec("SET a 1");
    EXPECT_EQ(exec("EXISTS a"), "1");
}

TEST_F(DBTest, Del) {
    exec("SET a 1");
    EXPECT_EQ(exec("DEL a"), "1");
}

TEST_F(DBTest, DBSize) {
    exec("SET a 1");
    exec("SET b 2");
    EXPECT_EQ(exec("DBSIZE"), "2");
}

TEST_F(DBTest, FlushDB) {
    exec("SET a 1");
    exec("FLUSHDB");
    EXPECT_EQ(exec("DBSIZE"), "0");
}

TEST_F(DBTest, MemoryUsage) {
    exec("SET a hello");
    EXPECT_FALSE(exec("MEMORY USAGE a").empty());
}

TEST_F(DBTest, ConfigSetGet) {
    EXPECT_EQ(exec("CONFIG SET maxmemory 1000"), "OK");
    EXPECT_TRUE(exec("CONFIG GET maxmemory").find("maxmemory") != std::string::npos);
}

TEST_F(DBTest, Expire) {
    exec("SET a 1");
    EXPECT_EQ(exec("EXPIRE a 10"), "1");
}

TEST_F(DBTest, TTLNoExpire) {
    exec("SET a 1");
    EXPECT_EQ(exec("TTL a"), "-1");
}

TEST_F(DBTest, TTLExpired) {
    exec("SET a 1");
    exec("EXPIRE a 0");
    EXPECT_EQ(exec("TTL a"), "-2");
}

TEST_F(DBTest, OOMWorks) {
    exec("CONFIG SET maxmemory 1");
    EXPECT_TRUE(exec("SET a hello").find("OOM") != std::string::npos);
}