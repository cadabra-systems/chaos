/**
 @file StringNode.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "StringNode.hpp"

namespace chaos { namespace cxml {
	string_node::string_node(const std::string value)
	:
		_value(value)
	{
	}
	std::ostream& string_node::feed(std::ostream& stream) const
	{
		stream << _value << std::endl;
		
		return stream;
	}
	
	void string_node::set_value(std::string value)
	{
		_value = value;
	}
	
	std::string string_node::get_value() const
	{
		return _value;
	}
} }
