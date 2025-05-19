/**
 @file Variant.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_Variant_hpp
#define Chaos_Flex_Variant_hpp

#include "../Flex.hpp"
#include <variant>

namespace chaos {
	template <>
	struct adl_serializer<std::monostate>
	{
		static void to_flex(flex& output, const std::monostate& input)
		{
			output = nullptr;
		}

		static void from_flex(const flex& input, std::monostate& output)
		{

		}
	};
}

#endif
