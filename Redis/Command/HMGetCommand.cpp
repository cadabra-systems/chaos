/**
 @file HMGetCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "HMGetCommand.hpp"

namespace chaos { namespace redis {
	hmget_command::hmget_command(std::string key, std::list<const std::string>& list)
	:
		_key(key),
		_list(list)
	{
		
	}
	
	hmget_command::hmget_command(std::string key, const std::string& member)
	:
		_key(key),
		_list({member})
	{
		
	}
		
	hmget_command::~hmget_command()
	{

	}

	command_line hmget_command::make_line() const
	{
		return command_line("HMGET", _key, _list);
	}
	
	std::size_t hmget_command::get_count() const
	{
		if (get_reply().get_type() != reply::type::array) {
			return 0;
		}
		
		return get_reply().get_count();
	};
} }

