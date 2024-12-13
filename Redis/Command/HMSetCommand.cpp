/**
 @file HMSetCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "HMSetCommand.hpp"

namespace chaos { namespace redis {
	hmset_command::hmset_command(std::string key, std::unordered_map<std::string, const std::string> map)
	:
		_key(key),
		_map(map)
	{
		
	}
	
	hmset_command::hmset_command(std::string key, const std::string& member, const std::string& value)
	:
		_key(key),
		_map({{member, value}})
	{
		
	}
	
	hmset_command::hmset_command(std::string key, const std::pair<const std::string, const std::string>& pair)
	:
		_key(key),
		_map({pair})
	{
		
	}
	
	hmset_command::~hmset_command()
	{

	}

	command_line hmset_command::make_line() const
	{
		return command_line("HMSET", _key, _map);
	}
	
	bool hmset_command::is_completed() const
	{
		if (get_reply() != reply::type::string) {
			return false;
		}
		
		std::string ok(get_reply().get_string());
		
		return ok.compare("OK") == 0;
	};
} }

