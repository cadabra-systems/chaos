/**
 @file Any.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_Any_hpp
#define Chaos_Flex_Any_hpp

#include "../Flex.hpp"

#include "../Any.hpp"

#include <any>

namespace chaos {
/*
	void to_flex(flex& output, const any& input)
	{
		output = input.flex();
	}
	void from_flex(const flex& input, any& output)
	{
		/// @todo not implemented
	}
*/

	template <>
	struct adl_serializer<std::any>
	{
		static inline void to_flex(flex& output, const std::any& input)
		{
			if (typeid(bool) == input.type()) {
				output = std::any_cast<bool>(input);
			} else if (typeid(double) == input.type()) {
				output = std::any_cast<float>(input);
			} else if (typeid(std::uint64_t) == input.type()) {
				output = std::any_cast<std::uint64_t>(input);
			} else if (typeid(std::int64_t) == input.type()) {
				output = std::any_cast<std::int64_t>(input);
			} else if (typeid(std::string) == input.type()) {
				output = std::any_cast<std::string>(input);
			} else if (typeid(nullptr_t) == input.type()) {
				output = nullptr;
			} else if (typeid(chaos::flex) == input.type()) {
				output = std::any_cast<chaos::flex>(input);
			} else {
				output = {};
			}
		}

		static inline void from_flex(const flex& input, std::any& output)
		{
			switch (input.type()) {
				case flex::flex_value_t::boolean:
					output = input.get<bool>();
					break;

				case flex::flex_value_t::null:
					output = nullptr;
					break;

				case flex::flex_value_t::number_float:
					output = input.get<double>();
					break;

				case flex::flex_value_t::number_integer:
					output = input.get<std::int64_t>();
					break;

				case flex::flex_value_t::number_unsigned:
					output = input.get<std::uint64_t>();
					break;

				case flex::flex_value_t::string:
					output = input.get<std::string>();
					break;

				case flex::flex_value_t::object:
					output = input;
					break;

				default:
					output.reset();
			}
		}
	};
}

#endif
