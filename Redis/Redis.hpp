/**
 * @file Redis.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Redis_hpp
#define Chaos_Redis_Redis_hpp

#include <hiredis/hiredis.h>

#include <memory>

namespace chaos { namespace redis {
	using context_ptr = std::unique_ptr<redisContext, decltype(&redisFree)>;
	inline context_ptr make_context(redisContext* raw_pointer) noexcept
	{
		return context_ptr{raw_pointer, redisFree};
	}

	using reply_ptr = std::unique_ptr<redisReply, decltype(&freeReplyObject)>;
	inline reply_ptr make_reply(redisReply* raw_pointer) noexcept
	{
		return reply_ptr{raw_pointer, freeReplyObject};
	}
	inline reply_ptr make_reply(void* raw_pointer) noexcept
	{
		return reply_ptr{static_cast<redisReply*>(raw_pointer), freeReplyObject};
	}
	inline reply_ptr make_reply(redisContext* context, const char* command) noexcept
	{
		return !context ? reply_ptr{nullptr, freeReplyObject} : reply_ptr{static_cast<redisReply*>(redisCommand(context, command)), freeReplyObject};
	}
	inline reply_ptr make_reply(redisContext* context, const char* command, va_list argument_list) noexcept
	{
		return !context ? reply_ptr{nullptr, freeReplyObject} : reply_ptr{static_cast<redisReply*>(redisvCommand(context, command, argument_list)), freeReplyObject};
	}
} }

#endif
