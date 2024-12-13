/**
 @file AuthCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "AuthCommand.hpp"

namespace chaos { namespace redis {
	auth_command::auth_command(std::string password)
	:
		_password(password)
	{
		
	}
	
	bool auth_command::is_success() const
	{
		if (get_reply() != reply::type::string) {
			return false;
		}
		
		return get_reply().get_string().compare("OK") == 0;
	};
} }
