/**
 @file AuthenticateCommand.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "AuthenticateCommand.hpp"

namespace chaos { namespace redis {
	authenticate_command::authenticate_command(const std::string& secret)
	:
		function<void>(),

		_password(secret)
	{

	}

	authenticate_command::authenticate_command(const std::string& username, const std::string& password)
	:
		function<void>(),

		_username(username),
		_password(password)
	{

	}

	procedure::state authenticate_command::execute(redisContext* context)
	{
		return _username.empty()
		? procedure::execute(context, "AUTH %s", _password.data())
		: procedure::execute(context, "AUTH %s %s", _username.data(), _password.data())
		;
	}
} }
