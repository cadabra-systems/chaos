/**
 @file SetClientNameCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "SetClientNameCommand.hpp"

namespace chaos { namespace redis {
	set_client_name_command::set_client_name_command(const std::string& value)
	:
		function<void>(),

		_value(value)
	{

	}

	procedure::status set_client_name_command::execute(redisContext* context)
	{
		return procedure::execute(context, "CLIENT SETNAME %s", _value.data());
	}
} }
