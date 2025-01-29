/**
 @file BigSignedInteger.сpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#include "BigSignedInteger.hpp"

namespace chaos { namespace cdo {
	big_signed_integer::big_signed_integer(const std::string& name, const std::string& alias, bool nullable, std::int64_t value)
	:
	  abstract_field(name, alias, nullable),
	  _value(value)
	{
		_type = abstract_field::fieldType::big_signed_integer;
	}

	big_signed_integer::big_signed_integer(const std::string& name, const std::string& alias, const std::string& rawExpression, bool nullable)
	:
	  abstract_field(name, alias, rawExpression, nullable)
	{
		_type = abstract_field::fieldType::big_signed_integer;
	}

	std::int64_t big_signed_integer::get_value() const
	{
		return _value;
	}

} }
