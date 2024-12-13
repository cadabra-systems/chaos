/**
 @file DoubleAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "DoubleAttribute.hpp"

namespace chaos { namespace cxml {
	double_attribute::double_attribute(const std::string name, const double value)
	:
		abstract_attribute(name),
		_value(value)
	{
		
	}
	
	void double_attribute::set_value(const double value)
	{
		_value = value;
	}
	
	const std::string double_attribute::get_value() const
	{
		return std::to_string(_value);
	}
} }
