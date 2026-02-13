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
		_lifetime(lifetime)
	{

	}

	procedure::status expire_command::execute(redisContext* context)
	{
		return procedure::execute(context, "PEXPIRE %s %d", _key.data(), _lifetime.count());
	}
} }
