/**
 @file PersistCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "PersistCommand.hpp"

namespace chaos { namespace redis {
	persist_command::persist_command(const std::string& key)
	:
		function<void>(),

		_key(key)
	{

	}

	procedure::status persist_command::execute(redisContext* context)
	{
		return procedure::execute(context, "PERSIST");
	}
} }
