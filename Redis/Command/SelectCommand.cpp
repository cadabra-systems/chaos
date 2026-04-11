/**
 @file SelectCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "SelectCommand.hpp"

namespace chaos { namespace redis {
	select_command::select_command(std::uint8_t index)
	:
		function<void>(),

		_index(std::to_string(index))
	{

	}

	procedure::status select_command::execute(redisContext* context)
	{
		return procedure::execute(context, "SELECT %s", _index.data());
	}
} }
