/**
 @file String.cpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#include "String.hpp"

#include <sstream>

namespace chaos { namespace cdo {

	string::string(const std::string& name, const std::string& alias, bool nullable, const std::string& value, const uint8_t& length)
	:
	  abstract_field(name, alias, nullable),
	  _value(value),
	  _length(length)

	{

	}

	std::string string::to_SQL()
	{
		std::ostringstream out;

		out << _name << " VARCHAR(" << static_cast<int>(_length )<< ")";

		if(!_nullable) {
			out << " NOT NULL";
		}

		return out.str();
	}

	std::string string::get_value() const
	{
		return _value;
	}

	bool string::isRaw() const
	{
		return _isRaw;
	}
} }
