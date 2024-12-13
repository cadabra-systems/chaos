/**
 @file ExistsCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "ExistsCommand.hpp"

namespace chaos { namespace redis {
	exists_command::exists_command(std::string key)
	:
		_key(key)
	{
		
	}
	
	exists_command::~exists_command()
	{

	}

	command_line exists_command::make_line() const
	{
		return command_line("EXISTS", _key);
	}

	bool exists_command::get() const
	{
		if (get_reply() != reply::type::integer) {
			return false;
		}
		
		return (1 == get_reply().get_integer());
	};
} }

