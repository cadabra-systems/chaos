/**
 @file DelCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "DelCommand.hpp"

namespace chaos { namespace redis {
	del_command::del_command(std::string key)
	:
		_key(key)
	{
		
	}
	
	del_command::~del_command()
	{
		
	}
	
	command_line del_command::make_line() const
	{
		return command_line("DEL", _key);
	}
	
	bool del_command::is_completed() const
	{
		if (get_reply() != reply::type::integer) {
			return false;
		}
		
		return (1 == get_reply().get_integer());
	}
} }

