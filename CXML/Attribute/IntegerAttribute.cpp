/**
 @file IntegerAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "IntegerAttribute.hpp"

namespace chaos { namespace cxml {
	integer_attribute::integer_attribute(const std::string name, const int value)
	:
		abstract_attribute(name),
		_value(value)
	{
		
	}
	
	void integer_attribute::set_value(const int value)
	{
		_value = value;
	}
	
	const std::string integer_attribute::get_value() const
	{
		return std::to_string(_value);
	}
} }
