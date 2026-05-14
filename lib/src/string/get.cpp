#include "DataBase.h"
#include <vector>


CommandResult DataBase::Get(const std::vector<std::string>& args) {

}

// unknown command	(error) unknown command
// wrong arg count	(error) wrong number of arguments for 'CMD'
// wrong type	(error) WRONGTYPE Operation against a key holding the wrong kind of value
// OOM	(error) OOM command not allowed when used memory > 'maxmemory'
// GET нет ключа	(nil)
// EXISTS нет ключа	0
// TTL нет ключа	-2