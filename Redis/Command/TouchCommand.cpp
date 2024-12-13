/**
 @file TouchCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "TouchCommand.hpp"

namespace chaos { namespace redis {
	touch_command::touch_command(std::string key)
	:
		_key(key)
	{
		
	}
	
	touch_command::~touch_command()
	{

	}

	command_line touch_command::make_line() const
	{
		return command_line("TOUCH", _key);
	}
	
	bool touch_command::is_completed() const
	{
		if (get_reply() != reply::type::integer) {
			return false;
		}
		
		return (1 == get_reply().get_integer());
	}
} }

