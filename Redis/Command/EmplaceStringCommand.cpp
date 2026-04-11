/**
 @file EmplaceStringCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "EmplaceStringCommand.hpp"

namespace chaos { namespace redis {
	emplace_string_command::emplace_string_command(const std::string& key, const std::string& value, const std::chrono::milliseconds& lifetime)
	:
		function<void>(),

		_key(key),
		_value(value),
		_lifetime(lifetime.count() > 0 ? std::to_string(lifetime.count()) : std::string{})
	{

	}

	procedure::status emplace_string_command::execute(redisContext* context)
	{
		return !_lifetime.empty()
		? procedure::execute(context, "SET %s %s NX PX %s", _key.data(), _value.data(), _lifetime.data())
		: procedure::execute(context, "SET %s %s NX", _key.data(), _value.data())
		;
	}
} }
