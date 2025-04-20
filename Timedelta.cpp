//
//  Timedela.cpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 15.04.21.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#include "Timedelta.hpp"

namespace chaos {
	const bimap<timedelta::interval, std::string> timedelta::interval_map =
	{
		{
			{timedelta::interval::year, "years"},
			{timedelta::interval::month, "months"},
			{timedelta::interval::day, "days"},
			{timedelta::interval::hour, "hours"},
			{timedelta::interval::minute, "minutes"},
			{timedelta::interval::second, "seconds"},
			{timedelta::interval::millisecond, "milliseconds"},
			{timedelta::interval::microsecond, "microseconds"}
		},
		timedelta::interval::microsecond
	};

	const std::array<timedelta::interval, 8> timedelta::interval_array
	{
		interval::year,
		interval::month,
		interval::day,
		interval::hour,
		interval::minute,
		interval::second,
		interval::millisecond,
		interval::microsecond
	};

	timedelta::timedelta
	(
		std::int16_t year,
		std::int16_t month,
		std::int16_t day,
		std::int16_t hour,
		std::int16_t minute,
		std::int16_t second,
		std::int16_t millisecond,
		std::int16_t microsecond
	)
	:
		_array
		(
			{
				year,
				month,
				day,
				hour,
				minute,
				second,
				millisecond,
				microsecond
			}
		)
	{

	}

	timedelta::timedelta(const timedelta& origin)
	:
		_array(origin._array)
	{

	}

	timedelta::timedelta(timedelta&& origin)
	:
		_array(std::move(origin._array))
	{

	}

	std::int16_t timedelta::operator[](interval i) const
	{
		return _array.at(static_cast<std::uint8_t>(i));
	}

	std::int16_t& timedelta::operator[](interval i)
	{
		return _array.at(static_cast<std::uint8_t>(i));
	}

	const std::array<std::int16_t, 8>& timedelta::get_array() const
	{
		return _array;
	}

	bool timedelta::is_zero() const
	{
		return std::all_of
		(
			std::cbegin(_array),
			std::cend(_array),
			[](const std::int16_t& value) { return 0 == value; }
		);
	}
}
