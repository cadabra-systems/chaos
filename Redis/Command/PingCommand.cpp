/**
 @file PingCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "PingCommand.hpp"

namespace chaos { namespace redis {
	ping_command::ping_command()
	:
		function<void>()
	{

	}

	procedure::state ping_command::execute(redisContext* context)
	{
		return procedure::execute(context, "PING");
	}
} }
