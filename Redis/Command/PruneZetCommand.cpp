/**
 @file PruneZetCommand.cpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "PruneZetCommand.hpp"

#include <limits>

namespace chaos { namespace redis {
	prune_zet_command::prune_zet_command(const std::string& key, std::int64_t min_score, std::int64_t max_score)
	:
		function<void>(),

		_key(key),
		_min_score(min_score),
		_max_score(max_score)
	{

	}

	procedure::status prune_zet_command::execute(redisContext* context)
	{
		const bool min_infinity(_min_score == std::numeric_limits<std::int64_t>::min());
		const bool max_infinity(_max_score == std::numeric_limits<std::int64_t>::max());
		if (min_infinity && max_infinity) {
			return procedure::execute(context, "ZREMRANGEBYSCORE %s -inf +inf", _key.data());
		} else if (min_infinity) {
			const std::string max_str(std::to_string(_max_score));
			return procedure::execute(context, "ZREMRANGEBYSCORE %s -inf %s", _key.data(), max_str.data());
		} else if (max_infinity) {
			const std::string min_str(std::to_string(_min_score));
			return procedure::execute(context, "ZREMRANGEBYSCORE %s %s +inf", _key.data(), min_str.data());
		}
		const std::string min_str(std::to_string(_min_score));
		const std::string max_str(std::to_string(_max_score));
		return procedure::execute(context, "ZREMRANGEBYSCORE %s %s %s", _key.data(), min_str.data(), max_str.data());
	}
} }
