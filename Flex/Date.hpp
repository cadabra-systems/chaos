#ifndef Chaos_Flex_Date_hpp
#define Chaos_Flex_Date_hpp

#include "../Flex.hpp"
#include "../Date.hpp"

namespace chaos {
	inline void to_flex(flex& output, const chaos::date& input)
	{
		output = chaos::flex {
			{"day", input.get_day()},
			{"month", input.get_month()},
			{"year", input.get_year()}
		};
	}

	inline void from_flex(const flex& input, chaos::date& output)
	{
		if (input["day"].is_number_integer()) {
			output.set_day(input["day"].get<int>());
		}

		if (input["month"].is_number_integer()) {
			output.set_month(input["month"].get<int>());
		}

		if (input["year"].is_number_integer()) {
			output.set_year(input["year"].get<int>());
		}
	}
}

#endif
