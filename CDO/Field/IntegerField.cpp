/**
 @file IntegerField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
*/

#include "IntegerField.hpp"

namespace chaos { namespace cdo {
	signed_integer::signed_integer(const std::string& name, const std::string &alias, bool nullable, std::int32_t value)
	:
		abstract_field(name, alias, nullable),

		_value(value)
	{
		_type = abstract_field::fieldType::signed_integer;
	}

	signed_integer::signed_integer(const std::string& name, const std::string& alias, const std::string& rawExpression, bool nullable)
	:
		abstract_field(name, alias, rawExpression, nullable)
	{
		_type = abstract_field::fieldType::signed_integer;
	}

	std::int32_t signed_integer::get_value() const
	{
		return _value;
	}

	void signed_integer::set_value(const std::int32_t& val)
	{
		_value = val;
	}
} }
