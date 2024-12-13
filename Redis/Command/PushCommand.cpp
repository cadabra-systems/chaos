/**
 @file PushCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "PushCommand.hpp"

namespace chaos { namespace redis {
	push_command::push_command(std::string key, std::initializer_list<std::string> list, push_command::list_ending ending)
	:
		_key(key),
		_list(list),
		_ending(ending)
	{
		
	}
	
	push_command::push_command(std::string key, std::string member, push_command::list_ending ending)
	:
		_key(key),
		_list({member}),
		_ending(ending)
	{
		
	}
	
	push_command::~push_command()
	{

	}

	command_line push_command::make_line() const
	{
		return command_line(static_cast<char>(_ending) + std::string("PUSH"), _key, _list);
	}
	
	std::size_t push_command::get_length() const
	{
		if (get_reply() != reply::type::integer) {
			return 0;
		}
		
		return get_reply().get_integer();
	}
} }

