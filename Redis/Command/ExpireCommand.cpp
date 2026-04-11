/**
 @file ExpireCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "ExpireCommand.hpp"

namespace chaos { namespace redis {
	expire_command::expire_command(const std::string& key, const std::chrono::milliseconds& lifetime)
	:
		function<void>(),

		_key(key),
		_lifetime(std::to_string(lifetime.count()))
	{

	}

	procedure::status expire_command::execute(redisContext* context)
	{
		return procedure::execute(context, "PEXPIRE %s %s", _key.data(), _lifetime.data());
	}
} }
