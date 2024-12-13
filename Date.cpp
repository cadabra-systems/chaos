#include "Date.hpp"
#include "Log.hpp"

#include <sstream>
#include <vector>

namespace chaos {
	date::date(int day, int month, int year)
	{
		set_day(day);
		set_month(month);
		set_year(year);
	}

	date::date(const std::string& date_string)
	:
		date()
	{
		std::vector<std::string> string_values;
		std::stringstream stream(date_string);
		std::string part;

		while (std::getline(stream, part, '-')) {
			string_values.push_back(part);
		}

		if (string_values.size() != 3) {
			return;
		}

		set_day(string_values.at(0));
		set_month(string_values.at(1));
		set_year(string_values.at(2));
	}

	date::operator std::tm() const
	{
		std::tm tm{};

		tm.tm_mday = _day;
		tm.tm_mon = _month - 1;
		tm.tm_year = _year - 1900;

		return tm;
	}

	date::operator std::string() const
	{
		std::string result;

		if (is_day_valid()) {
			std::string day(std::to_string(_day));
			if (1 <= _day && _day <= 9) {
				day.insert(0, "0");
			}
			result += day + "-";
		} else if (is_day_any()) {
			result += "**-";
		} else {
			result += "##-";
		}

		if (is_month_valid()) {
			std::string month(std::to_string(_month));
			if (1 <= _month && _month <= 9) {
				month.insert(0, "0");
			}
			result += month + "-";
		} else if (is_month_any()) {
			result += "**-";
		} else {
			result += "##-";
		}

		if (is_year_valid()) {
			result += std::to_string(_year);
		} else if (is_year_any()) {
			result += "****";
		} else {
			result += "####";
		}

		return result;
	}

	bool date::operator==(const date& rhs) const
	{
		if ((is_any() && rhs.is_undefined()) || (is_undefined() && rhs.is_any())) {
			return false;
		}

		bool result(true);

		if (is_day_valid() && rhs.is_day_valid()) {
			result = _day == rhs.get_day();
		}

		if ((is_day_valid() && rhs.is_day_undefined()) || (is_day_undefined() && rhs.is_day_valid())) {
			result = false;
		}

		if (!result) {
			return false;
		}

		if (is_month_valid() && rhs.is_month_valid()) {
			result = _month == rhs.get_month();
		}

		if ((is_month_valid() && rhs.is_month_undefined()) || (is_month_undefined() && rhs.is_month_valid())) {
			result = false;
		}

		if (!result) {
			return false;
		}

		if (is_year_valid() && rhs.is_year_valid()) {
			result = _year == rhs.get_year();
		}

		if ((is_year_valid() && rhs.is_year_undefined()) || (is_year_undefined() && rhs.is_year_valid())) {
			result = false;
		}

		return result;
	}

	bool date::operator!=(const date& rhs) const
	{
		if ((is_any() && rhs.is_undefined()) || (is_undefined() && rhs.is_any())) {
			return false;
		}

		bool result(false);

		if (is_day_valid() && rhs.is_day_valid()) {
			result = _day != rhs.get_day();
		}

		if ((is_day_valid() && rhs.is_day_undefined()) || (is_day_undefined() && rhs.is_day_valid())) {
			result = true;
		}

		if (result) {
			return true;
		}

		if (is_month_valid() && rhs.is_month_valid()) {
			result = _month != rhs.get_month();
		}

		if ((is_month_valid() && rhs.is_month_undefined()) || (is_month_undefined() && rhs.is_month_valid())) {
			result = true;
		}

		if (result) {
			return true;
		}

		if (is_year_valid() && rhs.is_year_valid()) {
			result = _year != rhs.get_year();
		}

		if ((is_year_valid() && rhs.is_year_undefined()) || (is_year_undefined() && rhs.is_year_valid())) {
			result = true;
		}

		return result;
	}

	bool date::operator>(const date& rhs) const
	{
		if ((is_any() && rhs.is_undefined()) || (is_undefined() && rhs.is_any())) {
			return false;
		}

		bool result(false);

		if (is_year_valid() && rhs.is_year_valid()) {
			result = _year > rhs.get_year();
		}

		if (_year != rhs.get_year()) {
			return result;
		}

		if (is_month_valid() && rhs.is_month_valid()) {
			result = _month > rhs.get_month();
		}

		if (_month != rhs.get_month()) {
			return result;
		}

		if (is_day_valid() && rhs.is_day_valid()) {
			result = _day > rhs.get_day();
		}

		return result;
	}

	bool date::operator>=(const date& rhs) const
	{
		if ((is_any() && rhs.is_undefined()) || (is_undefined() && rhs.is_any())) {
			return false;
		}

		bool result(true);

		if (is_year_valid() && rhs.is_year_valid()) {
			result = _year >= rhs.get_year();
		}

		if ((is_year_valid() && rhs.is_year_undefined()) || (is_year_undefined() && rhs.is_year_valid())) {
			result = false;
		}

		if (_year != rhs.get_year()) {
			return result;
		}

		if (is_month_valid() && rhs.is_month_valid()) {
			result = _month >= rhs.get_month();
		}

		if ((is_month_valid() && rhs.is_month_undefined()) || (is_month_undefined() && rhs.is_month_valid())) {
			result = false;
		}

		if (_month != rhs.get_month()) {
			return result;
		}

		if (is_day_valid() && rhs.is_day_valid()) {
			result = _day >= rhs.get_day();
		}

		if ((is_day_valid() && rhs.is_day_undefined()) || (is_day_undefined() && rhs.is_day_valid())) {
			result = false;
		}

		return result;
	}

	bool date::operator<(const date& rhs) const
	{
		if ((is_any() && rhs.is_undefined()) || (is_undefined() && rhs.is_any())) {
			return false;
		}

		bool result(false);

		if (is_year_valid() && rhs.is_year_valid()) {
			result = _year < rhs.get_year();
		}

		if (_year != rhs.get_year()) {
			return result;
		}

		if (is_month_valid() && rhs.is_month_valid()) {
			result = _month < rhs.get_month();
		}

		if (_month != rhs.get_month()) {
			return result;
		}

		if (is_day_valid() && rhs.is_day_valid()) {
			result = _day < rhs.get_day();
		}

		return result;
	}

	bool date::operator<=(const date& rhs) const
	{
		if ((is_any() && rhs.is_undefined()) || (is_undefined() && rhs.is_any())) {
			return false;
		}

		bool result(true);

		if (is_year_valid() && rhs.is_year_valid()) {
			result = _year <= rhs.get_year();
		}

		if ((is_year_valid() && rhs.is_year_undefined()) || (is_year_undefined() && rhs.is_year_valid())) {
			result = false;
		}

		if (_year != rhs.get_year()) {
			return result;
		}

		if (is_month_valid() && rhs.is_month_valid()) {
			result = _month <= rhs.get_month();
		}

		if ((is_month_valid() && rhs.is_month_undefined()) || (is_month_undefined() && rhs.is_month_valid())) {
			result = false;
		}

		if (_month != rhs.get_month()) {
			return result;
		}

		if (is_day_valid() && rhs.is_day_valid()) {
			result = _day <= rhs.get_day();
		}

		if ((is_day_valid() && rhs.is_day_undefined()) || (is_day_undefined() && rhs.is_day_valid())) {
			result = false;
		}

		return result;
	}

	std::ostream& operator<<(std::ostream& stream, const date& rhs)
	{
		stream << rhs.operator std::string();
		return stream;
	}

	void date::set_day(int day)
	{
		if (0 <= day && day <= 31) {
			_day = day;
		} else {
			_day = -1;
		}
	}

	void date::set_month(int month)
	{
		if (0 <= month && month <= 12) {
			_month = month;
		} else {
			_month = -1;
		}
	}

	void date::set_year(int year)
	{
		if (0 <= year && year <= 9999) {
			_year = year;
		} else {
			_year = -1;
		}
	}

	void date::set_day(const std::string& day)
	{
		try {
			int integer_day(std::stoi(day));
			set_day(integer_day);
		} catch (const std::invalid_argument& exception) {
			if (day == "**") {
				_day = 0;
			} else if (day == "##") {
				_day = -1;
			} else {
				chaos::log_register<chaos::log>::warning("date::set_day(const std::string& day) > ", day, " is not a valid integer value, an asterisk sign or a number sign");
				_day = -1;
			}
		} catch (const std::out_of_range& exception) {
			chaos::log_register<chaos::log>::warning("date(const std::string&) > ", day, " is out of integer type range");
			_day = -1;
		}
	}

	void date::set_month(const std::string& month)
	{
		try {
			int integer_month(std::stoi(month));
			set_month(integer_month);
		} catch (const std::invalid_argument& exception) {
			if (month == "**") {
				_month = 0;
			} else if (month == "##") {
				_month = -1;
			} else {
				chaos::log_register<chaos::log>::warning("date::set_month(const std::string& month) > ", month, " is not a valid integer value, an asterisk sign or a number sign");
				_month = -1;
			}
		} catch (const std::out_of_range& exception) {
			chaos::log_register<chaos::log>::warning("date::set_month(const std::string& month) > ", month, " is out of integer type range");
			_month = -1;
		}
	}

	void date::set_year(const std::string& year)
	{
		try {
			int integer_year(std::stoi(year));
			set_year(integer_year);
		} catch (const std::invalid_argument& exception) {
			if (year == "****") {
				_year = 0;
			} else if (year == "####") {
				_year = -1;
			} else {
				chaos::log_register<chaos::log>::warning("date::set_year(const std::string& year) > ", year, " is not a valid integer value, an asterisk sign or a number sign");
				_year = -1;
			}
		} catch (const std::out_of_range& exception) {
			chaos::log_register<chaos::log>::warning("date::set_year(const std::string& year) > ", year, " is out of integer type range");
			_year = -1;
		}
	}

	int date::get_day() const
	{
		return _day;
	}

	int date::get_month() const
	{
		return _month;
	}

	int date::get_year() const
	{
		return _year;
	}

	bool date::is_day_valid() const
	{
		return _day > 0;
	}

	bool date::is_month_valid() const
	{
		return _month > 0;
	}

	bool date::is_year_valid() const
	{
		return _year > 0;
	}

	bool date::is_day_any() const
	{
		return _day == 0;
	}

	bool date::is_month_any() const
	{
		return _month == 0;
	}

	bool date::is_year_any() const
	{
		return _year == 0;
	}

	bool date::is_day_undefined() const
	{
		return _day < 0;
	}

	bool date::is_month_undefined() const
	{
		return _month < 0;
	}

	bool date::is_year_undefined() const
	{
		return _year < 0;
	}

	bool date::is_valid() const
	{
		return is_day_valid() && is_month_valid() && is_year_valid();
	}

	bool date::is_any() const
	{
		return is_day_any() || is_month_any() || is_year_any();
	}

	bool date::is_undefined() const
	{
		return is_day_undefined() || is_month_undefined() || is_year_undefined();
	}
}
