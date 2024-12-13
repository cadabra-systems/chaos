//
//  Timedela.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 15.04.21.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Timedelta_hpp
#define Chaos_Timedelta_hpp

#include <map>
#include <list>
#include <array>
#include <string>
#include <ctime>
#include <chrono>
#include <cstdint>
#include <cmath>

namespace chaos {
	struct timedelta
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class interval : char
		{
			year = 0,
			month = 1,
			day = 2,
			hour = 3,
			minute = 4,
			second = 5,
			millisecond = 6,
			microsecond = 7
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		using interval_map = std::map<interval, std::int16_t>;
		using list_map = std::map<bool, std::list<std::string>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
/*
		timedelta()
		:
			year(0),
			month(0),
			day(0),
			hour(0),
			minute(0),
			second(0),
			millisecond(0),
			microsecond(0)
		{

		}

		timedelta(std::uint16_t year = 0, std::uint16_t month = 0, std::uint16_t day = 0, std::uint16_t hour = 0, std::uint16_t minute = 0, std::uint16_t second = 0, std::uint16_t millisecond = 0, std::uint16_t microsecond = 0)
		:
			year(0),
			month(0),
			day(0),
			hour(0),
			minute(0),
			second(0),
			millisecond(0),
			microsecond(0)
		{

		}

		timedelta(const timedelta& origin)
		:
			year(origin.year),
			month(origin.month),
			day(origin.day),
			hour(origin.hour),
			minute(origin.minute),
			second(origin.second),
			millisecond(origin.millisecond),
			microsecond(origin.microsecond)
		{

		}

		timedelta(timedelta&& origin)
		:
			year(std::move(origin.year)),
			month(std::move(origin.month)),
			day(std::move(origin.day)),
			hour(std::move(origin.hour)),
			minute(std::move(origin.minute)),
			second(std::move(origin.second)),
			millisecond(std::move(origin.millisecond)),
			microsecond(std::move(origin.microsecond))
		{

		}
*/
	/** @} */

	/** @name Properties */
	/** @{ */
	public:
		std::int16_t year = 0;
		std::int16_t month = 0;
		std::int16_t day = 0;
		std::int16_t hour = 0;
		std::int16_t minute = 0;
		std::int16_t second = 0;
		std::int16_t millisecond = 0;
		std::int16_t microsecond = 0;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		interval_map map() const
		{
			interval_map retval;
			if (0 != year) {
				retval.emplace(interval::year, year);
			}
			if (0 != month) {
				retval.emplace(interval::month, month);
			}
			if (0 != day) {
				retval.emplace(interval::day, day);
			}
			if (0 != hour) {
				retval.emplace(interval::hour, hour);
			}
			if (0 != minute) {
				retval.emplace(interval::minute, minute);
			}
			if (0 != second) {
				retval.emplace(interval::second, second);
			}
			if (0 != millisecond) {
				retval.emplace(interval::millisecond, millisecond);
			}
			if (0 != microsecond) {
				retval.emplace(interval::microsecond, microsecond);
			}
			return retval;
		}

		list_map list() const
		{
			std::array<std::pair<interval, std::int16_t>, 8> pair_array {
				{
					{interval::year, year},
					{interval::month, month},
					{interval::day, day},
					{interval::hour, hour},
					{interval::minute, minute},
					{interval::second, second},
					{interval::millisecond, millisecond},
					{interval::microsecond, microsecond}
				}
			};

			list_map retval;
			for (const std::pair<interval, std::int16_t>& pair : pair_array) {
				if (pair.second == 0) {
					continue;
				}
				std::list<std::string>& list(retval[(pair.second > 0)]);
				list.push_back(std::to_string(std::abs(pair.second)));
				switch (pair.first) {
					case chaos::timedelta::interval::year:
						list.back().append("years");
						break;
					case chaos::timedelta::interval::month:
						list.back().append("months");
						break;
					case chaos::timedelta::interval::day:
						list.back().append("days");
						break;
					case chaos::timedelta::interval::hour:
						list.back().append("hours");
						break;
					case chaos::timedelta::interval::minute:
						list.back().append("minutes");
						break;
					case chaos::timedelta::interval::second:
						list.back().append("seconds");
						break;
					case chaos::timedelta::interval::microsecond:
						list.back().append("microseconds");
						break;
					case chaos::timedelta::interval::millisecond:
						list.back().append("milliseconds");
						break;
				}
			}
			return retval;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		inline bool is_zero() const
		{
			return (0 == microsecond == millisecond == second == minute == hour == day == month == year);
		}
	/** @} */
	};
}

#endif
