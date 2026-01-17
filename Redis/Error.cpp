/**
 @file Error.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "Error.hpp"

#include <errno.h>

namespace chaos { namespace redis {
	error::error()
	:
		_code(code::other)
	{

	}

	error::error(error::code code, const std::string& message)
	:
		_code(code),
		_message(message)
	{

	}

	error::error(redisContext* context)
	:
		_code(static_cast<error::code>(context ? static_cast<code>(context->err) : code::other))
	{
		if (context) {
			_message = context->errstr;
			if (code::io == _code) {
				/// @brief If you included errno.h in your application, you can use the global errno variable to find out what is wrong
				_message.append("(" + std::string(strerror(errno)) + ")");
			}
		}
	}

	error::error(const context_ptr& context)
	:
		error(context.get())
	{

	}

	error::error(redisReply* reply)
	:
		_code(reply && REDIS_REPLY_ERROR == reply->type ? code::reply : code::other),
		_message(_code == code::reply  ? std::string{reply->str, reply->len}: "")
	{

	}

	error::error(const reply_ptr& reply)
	:
		error(reply.get())
	{

	}

	error::error(error&& origin)
	:
		_code(origin._code),
		_message(std::move(origin._message))
	{
		origin._code = code::other;
	}

	error::error(const error& origin)
	:
		_code(origin._code),
		_message(origin._message)
	{

	}

	error& error::operator=(error&& rhs)
	{
		_code = std::move(rhs._code);
		_message = std::move(rhs._message);
		rhs._code = code::other;
		return *this;
	}

	error& error::operator=(error const& rhs)
	{
		_code = rhs._code;
		_message = rhs._message;
		return *this;
	}

	error::code error::get_code() const
	{
		return _code;
	}

	std::string error::get_message() const
	{
		return _message;
	}
} }
