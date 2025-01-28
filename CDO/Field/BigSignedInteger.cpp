/**
 @file BigSignedInteger.сpp
 @date 17.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#include "BigSignedInteger.hpp"
#include <sstream>

namespace chaos { namespace cdo {
	big_signed_integer::big_signed_integer(const std::string& name, const std::string& alias, bool nullable, std::int64_t value)
	:
	  abstract_field(name, alias, nullable),
	  _value(value)
	{}

	big_signed_integer::big_signed_integer(const std::string& name, const std::string& alias, const std::string& rawExpression, bool nullable)
	:
	  abstract_field(name, alias, rawExpression, nullable)
	{}

	std::string big_signed_integer::to_SQL()
	{
		std::ostringstream out;

		out << _name << " BIGINT";

		if(!_nullable) {
			out << " NOT NULL";
		}

		return out.str();
	}

	std::int64_t big_signed_integer::get_value() const
	{
		return _value;
	}

} }
