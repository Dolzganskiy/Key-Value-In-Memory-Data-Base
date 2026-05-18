#include "DataBase.h"
#include <vector>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

CommandResult DataBase::FlushDB(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return std::unexpected("(error) wrong number of arguments for 'FLUSHDB'");
    }

    db_.clear();
    memory_usage_ = 0;

    return "OK";
}