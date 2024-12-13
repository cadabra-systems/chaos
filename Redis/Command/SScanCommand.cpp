/**
 @file SScanCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "SScanCommand.hpp"

namespace chaos { namespace redis {
	sscan_command::sscan_command(std::string key, std::int64_t cursor_id)
	:
		_key(key),
		_cursor_id(cursor_id)
	{
		
	}
	
	sscan_command::~sscan_command()
	{

	}

	command_line sscan_command::make_line() const
	{
		return command_line("SSCAN", _key, _cursor_id);
	}
	
	std::int64_t sscan_command::get_cursor_id() const
	{
		if (get_reply() != reply::type::array) {
			return false;
		}
		
		return get_reply()[0];
	}
} }

