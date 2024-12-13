/**
 @file UIDAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "UIDAttribute.hpp"

namespace chaos { namespace cxml {
	uid_attribute::uid_attribute(const std::string name, const chaos::uid value)
	:
		abstract_attribute(name),
		_value(value)
	{
		
	}
	
	void uid_attribute::set_value(const chaos::uid& value)
	{
		_value = value;
	}
	
	void uid_attribute::set_value(const chaos::uid value)
	{
		_value = value;
	}
	
	const std::string uid_attribute::get_value() const
	{
		return _value;
	}
} }
