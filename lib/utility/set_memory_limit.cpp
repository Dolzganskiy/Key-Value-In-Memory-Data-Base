#include "DataBase.h"

void DataBase::SetMemoryLimit(size_t limit) {
    if (limit == 0) {
        memory_limit_ = std::nullopt;
    } else {
        memory_limit_ = limit;
    }
}