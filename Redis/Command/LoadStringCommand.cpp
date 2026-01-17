/**
 @file LoadStringCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "LoadStringCommand.hpp"

namespace chaos { namespace redis {
	load_string_command::load_string_command(const std::string& key)
	:
		function<std::string>(),

		_key(key)
	{

	}

	procedure::state load_string_command::execute(redisContext* context)
	{
		return procedure::execute(context, "GET %s", _key.data());
	}
} }
