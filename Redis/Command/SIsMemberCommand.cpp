/**
 @file SIsMemberCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "SIsMemberCommand.hpp"

namespace chaos { namespace redis {
	sismember_command::sismember_command(std::string key, std::string value)
	:
		_key(key),
		_value(value)
	{
		
	}
	
	sismember_command::~sismember_command()
	{

	}

	command_line sismember_command::make_line() const
	{
		return command_line("SISMEMBER", _key, _value);
	}
	
	bool sismember_command::is_exists() const
	{
		if (get_reply() != reply::type::integer) {
			return false;
		}
		
		return get_reply().get_integer() == 1;
	}
} }

