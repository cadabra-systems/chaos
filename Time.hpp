//
//  Time.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Time_hpp
#define Chaos_Time_hpp

#include "String.hpp"

#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std::chrono_literals;

namespace chaos {
	class time
	{
	/** @name Constructors */
	/** @{ */
	public:
		time(bool now = false)
		:
			_year(0),
			_month(0),
			_day(0),
			_hour(0),
			_minute(0),
			_second(0),
			_fraction(0)
		{
			if (!now) {
				return;
			}

			std::chrono::system_clock::time_point tp(std::chrono::system_clock::now());
			std::time_t tt(std::chrono::system_clock::to_time_t(tp));
			std::tm tm{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			gmtime_r(&tt, &tm);
			const std::chrono::duration<double> td(tp - std::chrono::system_clock::from_time_t(tt));

			_month = tm.tm_mon + 1;
			_day = tm.tm_mday;
			_year = tm.tm_year + 1900;
			_hour = tm.tm_hour;
			_minute = tm.tm_min;
			_second = tm.tm_sec;
			_fraction = static_cast<std::uint32_t>(1000000 * (td.count()));
		}

		time(const time& origin)
		:
			_year(origin._year),
			_month(origin._month),
			_day(origin._day),
			_hour(origin._hour),
			_minute(origin._minute),
			_second(origin._second),
			_fraction(origin._fraction)
		{

		}

		time(time&& origin)
		:
			_year(std::move(origin._year)),
			_month(std::move(origin._month)),
			_day(std::move(origin._day)),
			_hour(std::move(origin._hour)),
			_minute(std::move(origin._minute)),
			_second(std::move(origin._second)),
			_fraction(std::move(origin._fraction))
		{

		}

		time(const std::chrono::system_clock::time_point& value)
		:
			time(std::chrono::system_clock::to_time_t(value))
		{

		}

		time(const std::time_t& value)
		{
			set(value);
		}

		time(const std::string& value)
		{
			set(value);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::uint16_t _year;
		std::uint8_t _month;
		std::uint8_t _day;
		std::uint8_t _hour;
		std::uint8_t _minute;
		std::uint8_t _second;
		std::uint32_t _fraction;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		operator std::string() const
		{
			/// @brief YYYY-MM-DD HH:MM:SS.SSSSSS
			return chaos::string::formatted_string("%d-%02d-%02d %02d:%02d:%02d.%06d", _year, _month, _day, _hour, _minute, _second, _fraction);
		}

		operator std::chrono::system_clock::time_point() const
		{
			std::time_t t = std::time(nullptr);
			std::tm timeinfo = *std::localtime(&t);
			timeinfo.tm_year = _year - 1900;
			timeinfo.tm_mon = _month - 1;
			timeinfo.tm_mday = _day;
			timeinfo.tm_hour = _hour;
			timeinfo.tm_min = _minute;
			timeinfo.tm_sec = _second;

			return std::chrono::system_clock::from_time_t(std::mktime(&timeinfo));
		}

		time& operator=(const std::chrono::system_clock::time_point& value)
		{
			set(value);

			return *this;
		}

		time& operator=(const time& origin)
		{
			_year = origin._year;
			_month = origin._month;
			_day = origin._day;
			_hour = origin._hour;
			_minute = origin._minute;
			_second = origin._second;
			_fraction = origin._fraction;

			return *this;
		}

		time& operator=(time&& origin)
		{
			_year = std::move(origin._year);
			_month = std::move(origin._month);
			_day = std::move(origin._day);
			_hour = std::move(origin._hour);
			_minute = std::move(origin._minute);
			_second = std::move(origin._second);
			_fraction = std::move(origin._fraction);

			return *this;
		}

		bool operator==(const time& rhs) const
		{
			return (_fraction != rhs._fraction) ? false : count_seconds_since_epoch() == rhs.count_seconds_since_epoch();
		}

		bool operator!=(const time& rhs) const
		{
			return (_fraction != rhs._fraction) ? true : count_seconds_since_epoch() != rhs.count_seconds_since_epoch();
		}

		bool operator>(const time& rhs) const
		{
			const std::int64_t left(count_seconds_since_epoch());
			const std::int64_t right(rhs.count_seconds_since_epoch());
			if (left > right || (left == right && _fraction > rhs._fraction)) {
				return true;
			}
			return false;
		}

		bool operator>=(const time& rhs) const
		{
			const std::int64_t left(count_seconds_since_epoch());
			const std::int64_t right(rhs.count_seconds_since_epoch());
			if (left > right || (left == right && _fraction >= rhs._fraction)) {
				return true;
			}
			return false;
		}

		bool operator<(const time& rhs) const
		{
			const std::int64_t left(count_seconds_since_epoch());
			const std::int64_t right(rhs.count_seconds_since_epoch());
			if (left < right || (left == right && _fraction < rhs._fraction)) {
				return true;
			}
			return false;
		}
		bool operator<=(const time& rhs) const
		{
			const std::int64_t left(count_seconds_since_epoch());
			const std::int64_t right(rhs.count_seconds_since_epoch());
			if (left < right || (left == right && _fraction <= rhs._fraction)) {
				return true;
			}
			return false;
		}

		time& operator+=(const std::chrono::microseconds& value)
		{
			add_fraction(static_cast<std::uint32_t>(value.count()));
			return *this;
		}

		time operator+(const std::chrono::microseconds& value) const
		{
			chaos::time retval(*this);
			retval.add_fraction(static_cast<std::uint32_t>(value.count()));
			return retval;
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set_year(std::uint16_t value)
		{
			_year = value;
		}

		void set_month(std::uint8_t value)
		{
			_month = value;
		}

		void set_day(std::uint8_t value)
		{
			_day = value;
		}

		void set_hour(std::uint8_t value)
		{
			_hour = value;
		}

		void set_minute(std::uint8_t value)
		{
			_minute = value;
		}

		void set_second(std::uint8_t value)
		{
			_second = value;
		}

		void set_fraction(std::uint32_t value)
		{
			_fraction = value;
			if (_fraction > 999999) {
				_fraction /= 1000;
			}
		}

		void add_fraction(std::uint32_t value)
		{
			_fraction += value;
			if (_fraction > 999999) {
				const std::lldiv_t d(std::lldiv(_fraction, 999999));
				std::chrono::system_clock::time_point t(*this);
				t += std::chrono::seconds(d.quot);
				_fraction = static_cast<std::uint32_t>(d.rem);
			}
		}

		void set(const std::time_t& value)
		{
			std::tm* tm = std::localtime(&value);
			set(*tm);
		}

		void set(const std::tm& value)
		{
			_year = static_cast<std::uint16_t>(value.tm_year + 1900);
			_month = static_cast<std::uint8_t>(value.tm_mon + 1);
			_day = static_cast<std::uint8_t>(value.tm_mday);
			_hour = static_cast<std::uint8_t>(value.tm_hour);
			_minute = static_cast<std::uint8_t>(value.tm_min);
			_second = static_cast<std::uint8_t>(value.tm_sec);
			_fraction = 0;
		}

		void set(const std::chrono::system_clock::time_point& value)
		{
			set(std::chrono::system_clock::to_time_t(value));
		}

		void set(const std::string& value)
		{
			if (value.empty()) {
				return ;
			}
			std::vector<std::string> v = string::explode<std::vector, std::string>(value, ".");

			std::tm tm = {};
			std::istringstream stream(v.front());
			const static std::locale default_locale("");
			stream.imbue(default_locale);
			stream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
			if (!stream.fail()) {
				set(tm);
				try {
					set_fraction((v.size() == 2) ? static_cast<std::uint32_t>(std::stoul(v.back())) : 0);
				} catch (...) {
					_fraction = 0;
				}
			}
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::uint16_t& get_year() const
		{
			return _year;
		}

		const std::uint8_t& get_month() const
		{
			return _month;
		}

		const std::uint8_t& get_day() const
		{
			return _day;
		}

		const std::uint8_t& get_hour() const
		{
			return _hour;
		}

		const std::uint8_t& get_minute() const
		{
			return _minute;
		}

		const std::uint8_t& get_second() const
		{
			return _second;
		}

		const std::uint32_t& get_fraction() const
		{
			return _fraction;
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		std::int64_t count_seconds_since_epoch() const
		{
			if (zero()) {
				return 0;
			}

			std::time_t rawtime;
			std::time(&rawtime);

			struct std::tm* timeinfo = std::localtime(&rawtime);
			timeinfo->tm_year = _year - 1900;
			timeinfo->tm_mon = _month - 1;
			timeinfo->tm_mday = _day;
			timeinfo->tm_hour = _hour;
			timeinfo->tm_min = _minute;
			timeinfo->tm_sec = _second;

			return static_cast<std::int64_t>(std::mktime(timeinfo));
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool zero() const
		{
			return (0 == _year && 0 == _month && 0 == _day && 0 == _hour && 0 == _minute && 0 == _second);
		}
	/** @} */
	};
}

#endif
