/**
 @file ExpireCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "ExpireCommand.hpp"

namespace chaos { namespace redis {
	expire_command::expire_command(std::string key, std::chrono::seconds seconds)
	:
		_key(key),
		_seconds(seconds)
	{
		
	}
	
	expire_command::~expire_command()
	{

	}

	command_line expire_command::make_line() const
	{
		return command_line("EXPIRE", _key, _seconds.count());
	}
	
	bool expire_command::is_completed() const
	{
		if (get_reply() != reply::type::integer) {
			return false;
		}
		
		return (1 == get_reply().get_integer());
	}
} }

