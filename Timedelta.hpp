//
//  Timedela.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 15.04.21.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Timedelta_hpp
#define Chaos_Timedelta_hpp

#include "BiMap.hpp"

#include <map>
#include <list>
#include <string>
#include <cstdint>

namespace chaos {
	struct timedelta
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class interval : std::uint8_t
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

	/** @name Statics*/
	/** @{ */
	public:
		static const bimap<timedelta::interval, std::string> interval_map;
		static const std::array<timedelta::interval, 8> interval_array;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		using map = std::map<bool, std::list<std::string>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		timedelta
		(
			std::int16_t year = 0,
			std::int16_t month = 0,
			std::int16_t day = 0,
			std::int16_t hour = 0,
			std::int16_t minute = 0,
			std::int16_t second = 0,
			std::int16_t millisecond = 0,
			std::int16_t microsecond = 0
		);
		timedelta(const timedelta& origin);
		timedelta(timedelta&& origin);
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::array<std::int16_t, 8> _array;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		std::int16_t operator[](interval i) const;
		std::int16_t& operator[](interval i);
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::array<std::int16_t, 8>& get_array() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_zero() const;
	/** @} */
	};
}

#endif
