/**
 @file ScanCommand.cpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "ScanCommand.hpp"

namespace chaos { namespace redis {
	scan_command::scan_command(const std::string& pattern, std::size_t count)
	:
		cursor<std::string>(),

		_pattern(pattern),
		_count(std::to_string(count))
	{

	}

	procedure::status scan_command::execute(redisContext* context)
	{
		const std::string offset_str(std::to_string(get_offset()));
		return procedure::execute(context, "SCAN %s MATCH %s COUNT %s", offset_str.data(), _pattern.data(), _count.data());
	}
} }
