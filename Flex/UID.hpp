/**
 @file UID.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_UID_hpp
#define Chaos_Flex_UID_hpp

#include "../Flex.hpp"
#include "../UID.hpp"

namespace chaos {
	inline void to_flex(flex& output, const uid& input)
	{
		output = static_cast<std::string>(input);
	}

	inline void from_flex(const flex& input, uid& output)
	{
		if (input.is_string()) {
			output.parse(input.get<std::string>());
		} else {
			output.zero();
		}
	}
}

#endif
