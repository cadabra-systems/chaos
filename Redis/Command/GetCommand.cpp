/**
 @file GetCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "GetCommand.hpp"

namespace chaos { namespace redis {
	get_command::get_command(std::string key)
	:
		_key(key)
	{
		
	}
	
	get_command::~get_command()
	{

	}

	command_line get_command::make_line() const
	{
		return command_line("GET", _key);
	}
} }

