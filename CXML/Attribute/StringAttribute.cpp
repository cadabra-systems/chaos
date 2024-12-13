/**
 @file StringAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "StringAttribute.hpp"

namespace chaos { namespace cxml {
	string_attribute::string_attribute(const std::string name, const std::string value)
	:
		abstract_attribute(name),
		_value(value)
	{
		
	}
	
	void string_attribute::set_value(const std::string value)
	{
		_value = value;
	}
	
	const std::string string_attribute::get_value() const
	{
		return _value;
	}
} }
