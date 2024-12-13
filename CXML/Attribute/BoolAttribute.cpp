/**
 @file BoolAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "BoolAttribute.hpp"

namespace chaos { namespace cxml {
	bool_attribute::bool_attribute(const std::string name, const bool yesno)
	:
		abstract_attribute(name),
		_yesno(yesno)
	{
		
	}
	
	void bool_attribute::set_value(const bool yesno)
	{
		_yesno = yesno;
	}
	
	const std::string bool_attribute::get_value() const
	{
		return _yesno ? "true" : "false";
	}
} }
