/**
 @file RealNumericAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "IndexAttribute.hpp"

namespace chaos { namespace cxml {
	index_attribute::index_attribute(const std::string name, const std::uint64_t value)
	:
		abstract_attribute(name),
		_value(value)
	{
		
	}
	
	void index_attribute::set_value(const std::uint64_t value)
	{
		_value = value;
	}
	
	const std::string index_attribute::get_value() const
	{
		return std::to_string(_value);
	}
} }
