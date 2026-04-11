/**
 @file ResetZetItemCommand.cpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "ResetZetItemCommand.hpp"

namespace chaos { namespace redis {
	reset_zet_item_command::reset_zet_item_command(const std::string& key, std::int64_t score, const std::string& member)
	:
		function<void>(),

		_key(key),
		_score(std::to_string(score)),
		_member(member)
	{

	}

	procedure::status reset_zet_item_command::execute(redisContext* context)
	{
		return procedure::execute(context, "ZADD %s XX %s %s", _key.data(), _score.data(), _member.data());
	}
} }
