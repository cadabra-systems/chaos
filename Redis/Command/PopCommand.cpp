/**
 @file PopCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "PopCommand.hpp"

namespace chaos { namespace redis {
	pop_command::pop_command(std::string key, list_ending ending)
	:
		_key(key),
		_ending(ending)
	{
		
	}
	
	pop_command::~pop_command()
	{

	}

	command_line pop_command::make_line() const
	{
		return command_line(static_cast<char>(_ending) + std::string("POP"), _key);
	}
	
	bool pop_command::has_element() const
	{
		if (get_reply() != reply::type::array) {
			return 0;
		}
		
		return get_reply().get_count();
	}
} }

