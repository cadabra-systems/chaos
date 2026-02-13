/**
 @file Command.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "Command.hpp"

namespace chaos { namespace redis {
	procedure::~procedure()
	{

	}

	procedure::status procedure::execute(redisContext* context, const char* command_name, ...)
	{
		va_list argument_list;
		va_start(argument_list, command_name);
		resolve(make_reply(context, command_name, argument_list));
		va_end(argument_list);
		return get_status();
	}
} }
