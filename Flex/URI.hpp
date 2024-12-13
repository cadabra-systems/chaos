/**
 @file URI.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_URI_hpp
#define Chaos_Flex_URI_hpp

#include "../Flex.hpp"
#include "../URI.hpp"

#include <string>

namespace chaos {
	inline void to_flex(flex& output, const uri& input)
	{
		output = static_cast<std::string>(input);
	}

	inline void from_flex(const flex& input, uri& output)
	{
		if (input.is_string()) {
			output.parse(input.get<std::string>());
		} else {
			output.clear();
		}
	}
}

#endif
