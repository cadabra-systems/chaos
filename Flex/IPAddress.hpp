/**
 @file IPAddress.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_IPAddress_hpp
#define Chaos_Flex_IPAddress_hpp

#include "../Flex.hpp"
#include "../IPAddress.hpp"

namespace chaos {
	void to_flex(chaos::flex& output, const chaos::ip_address& input)
	{
		output = input.make_string();
	}

	void from_flex(const chaos::flex& input, chaos::ip_address& output)
	{
		output.set(input.get<std::string>());
	}
}

#endif
