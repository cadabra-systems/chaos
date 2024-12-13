/**
 @file ExpireAtCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "ExpireAtCommand.hpp"

namespace chaos { namespace redis {
	expireat_command::expireat_command(std::string key, std::chrono::system_clock::time_point timestamp)
	:
		_key(key),
		_timestamp(timestamp)
	{
		
	}
	
	expireat_command::~expireat_command()
	{

	}

	command_line expireat_command::make_line() const
	{
		return command_line("EXPIREAT", _key, std::chrono::duration_cast<std::chrono::seconds>(_timestamp.time_since_epoch()).count());
	}
	
	bool expireat_command::is_completed() const
	{
		if (get_reply() != reply::type::integer) {
			return false;
		}
		
		return (1 == get_reply().get_integer());
	}
} }

