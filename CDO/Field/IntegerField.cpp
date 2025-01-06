/**
 @file IntegerField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
*/

#include "IntegerField.hpp"

#include <sstream>

namespace chaos { namespace cdo {
	signed_integer::signed_integer(const std::string& name, bool nullable, std::int32_t value)
	:
		abstract_field(name, nullable),

		_value(value)
	{

	}

	std::string signed_integer::to_SQL()
	{
		std::ostringstream out;

		out << _name << " INTEGER";

		if(!_nullable) {
			out << " NOT NULL";
		}

		return out.str();
	}

	std::int32_t signed_integer::get_value() const
	{
		return _value;
	}
} }
