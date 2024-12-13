/**
 @file DimensionAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "DimensionAttribute.hpp"

namespace chaos { namespace cxml {
	dimension_attribute::dimension_attribute(const std::string& name, const chaos::dimension& dimension)
	:
		abstract_attribute(name),
		_dimension(dimension)
	{
		
	}
	
	void dimension_attribute::set_value(const chaos::dimension::value_type value)
	{
		_dimension.set(value);
	}
	
	void dimension_attribute::set_unit(const chaos::dimension::unit_type type)
	{
		_dimension.set(type);
	}
	
	const std::string dimension_attribute::get_value() const
	{
		switch (_dimension.unit()) {
			case chaos::dimension::unit_type::ratio:
				return std::to_string(_dimension.value()) + "%";
				break;
				
			case chaos::dimension::unit_type::length:
				return std::to_string(_dimension.value());
				break;
				
			default:
				return "auto";
				break;
		}
	}
} }
