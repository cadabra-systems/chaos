/**
 @file AbstractAttribute.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "AbstractAttribute.hpp"

namespace chaos { namespace cxml {
	abstract_attribute::abstract_attribute(const std::string name)
	:
		_name(name)
	{
		
	}
	
	const std::string& abstract_attribute::get_name() const
	{
		return _name;
	}
} }
