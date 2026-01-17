/**
 @file ResetStringCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "ResetStringCommand.hpp"

namespace chaos { namespace redis {
	reset_string_command::reset_string_command
	(
		const std::string& key,
		const std::string& value,
		const std::chrono::milliseconds& lifetime
	)
	:
		function<void>(),

		_key(key),
		_value(value),
		_lifetime(lifetime)
	{

	}

	procedure::state reset_string_command::execute(redisContext* context)
	{
		return (_lifetime.count() > 0)
		? procedure::execute(context, "SET %s %s PX %lld", _key.data(), _value.data(), _lifetime.count())
		: procedure::execute(context, "SET %s %s", _key.data(), _value.data())
		;
	}
} }
