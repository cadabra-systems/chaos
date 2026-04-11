/**
 @file RemoveZetItemCommand.cpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "RemoveZetItemCommand.hpp"

namespace chaos { namespace redis {
	remove_zet_item_command::remove_zet_item_command(const std::string& key, const std::string& member)
	:
		function<bool>(),

		_key(key),
		_member(member)
	{

	}

	procedure::status remove_zet_item_command::execute(redisContext* context)
	{
		return procedure::execute(context, "ZREM %s %s", _key.data(), _member.data());
	}
} }
