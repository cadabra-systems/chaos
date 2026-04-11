/**
 @file UpdateZetItemCommand.cpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "UpdateZetItemCommand.hpp"

#include <cstdlib>

namespace chaos { namespace redis {
	update_zet_item_command::update_zet_item_command(const std::string& key, std::int64_t score, const std::string& member)
	:
		function<bool>(),

		_key(key),
		_score(std::to_string(score > 0 ? score : std::llabs(score))),
		_gl(score > 0),
		_member(member)
	{

	}

	procedure::status update_zet_item_command::execute(redisContext* context)
	{
		return _gl
		? procedure::execute(context, "ZADD %s XX CH GT %s %s", _key.data(), _score.data(), _member.data())
		: procedure::execute(context, "ZADD %s XX CH LT %s %s", _key.data(), _score.data(), _member.data())
		;
	}
} }
