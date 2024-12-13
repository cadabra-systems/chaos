/**
 @file TimePoint.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_TimePoint_hpp
#define Chaos_Flex_TimePoint_hpp

#include "../Flex.hpp"

#include <chrono>

namespace chaos {
	template <>
	struct adl_serializer<std::chrono::system_clock::time_point> {
		static void to_flex(flex& output, const std::chrono::system_clock::time_point& input)
		{
			output = std::chrono::duration_cast<std::chrono::seconds>(input.time_since_epoch()).count();
		}

		static void from_flex(const flex& input, std::chrono::system_clock::time_point& output)
		{
			if (input.is_number_integer()) {
				output = std::chrono::system_clock::time_point(std::chrono::seconds(input.get<std::int64_t>()));
			} else if (input.is_string()) {
				output =  std::chrono::system_clock::time_point(std::chrono::seconds(std::stoll(input.get<std::string>())));
			}
		}
	};
}

#endif
