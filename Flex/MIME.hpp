/**
 @file MIME.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_MIME_hpp
#define Chaos_Flex_MIME_hpp

#include "../Flex.hpp"
#include "../MIME.hpp"

#include <string>

namespace chaos {	
	inline void to_flex(chaos::flex& output, const mime& input)
	{
		output = static_cast<std::string>(input);
	}

	inline void from_flex(const chaos::flex& input, mime& output)
	{
		output.parse(input.get<std::string>());
	}
}

#endif
