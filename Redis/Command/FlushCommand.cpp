/**
 @file FlushCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "FlushCommand.hpp"

namespace chaos { namespace redis {
	flush_command::flush_command(bool async_mode)
	:
		function<void>(),

		_async_mode(async_mode)
	{

	}

	procedure::status flush_command::execute(redisContext* context)
	{
		return _async_mode ? procedure::execute(context, "FLUSHDB ASYNC") : procedure::execute(context, "FLUSHDB SYNC");
	}
} }
