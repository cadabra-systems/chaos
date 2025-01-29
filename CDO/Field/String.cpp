/**
 @file String.cpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#include "String.hpp"

namespace chaos { namespace cdo {

	string::string(const std::string& name, const std::string& alias, bool nullable, const std::string& value, const uint8_t& length)
	:
	  abstract_field(name, alias, nullable),
	  _value(value),
	  _length(length)
	{
		_type = abstract_field::fieldType::string;
	}

	string::string(const std::string& name, const std::string& alias, const std::string& rawExpression, bool nullable)
	:
	  abstract_field(name, alias, rawExpression, nullable)
	{
		_type = abstract_field::fieldType::string;
	}

	std::string string::get_value() const
	{
		return _value;
	}

	void string::set_value(const std::string& val)
	{
		_value = val;
		_length = val.length();
	}

} }
