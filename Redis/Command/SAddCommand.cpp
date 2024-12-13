/**
 @file SAddCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "SAddCommand.hpp"

namespace chaos { namespace redis {
	sadd_command::sadd_command(std::string key, std::initializer_list<std::string> list)
	:
		_key(key),
		_list(list)
	{
		
	}
	
	sadd_command::sadd_command(std::string key, std::string member)
	:
		_key(key),
		_list({member})
	{
		
	}
	
	sadd_command::~sadd_command()
	{

	}

	command_line sadd_command::make_line() const
	{
		return command_line("SADD", _key, _list);
	}
	
	std::size_t sadd_command::get_affected_members() const
	{
		if (get_reply() != reply::type::integer) {
			return 0;
		}
		
		return get_reply().get_integer();
	}
} }

