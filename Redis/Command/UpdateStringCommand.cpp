/**
 @file UpdateStringCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "UpdateStringCommand.hpp"

namespace chaos { namespace redis {
	update_string_command::update_string_command(const std::string& key, const std::string& expected_value, const std::string& desired_value, const std::chrono::milliseconds& lifetime)
	:
		function<void>(),

		_key(key),
		_expected_value(expected_value),
		_desired_value(desired_value),
		_lifetime(lifetime.count() > 0 ? std::to_string(lifetime.count()) : std::string{})
	{

	}

	procedure::status update_string_command::execute(redisContext* context)
	{
		if (_expected_value.empty()) {
			return !_lifetime.empty()
			? procedure::execute(context, "SET %s %s XX PX %s", _key.data(), _desired_value.data(), _lifetime.data())
			: procedure::execute(context, "SET %s %s XX", _key.data(), _desired_value.data())
			;
		}
		return !_lifetime.empty()
		? procedure::execute(context, "SET %s %s IFEQ %s PX %s", _key.data(), _desired_value.data(), _expected_value.data(), _lifetime.data())
		: procedure::execute(context, "SET %s %s IFEQ %s", _key.data(), _desired_value.data(), _expected_value.data())
		;
	}
} }
