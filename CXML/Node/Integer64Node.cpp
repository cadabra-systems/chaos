/**
 @file Integer64Node.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Integer64Node.hpp"

namespace chaos { namespace cxml {
	integer64_node::integer64_node(const std::int64_t value)
	:
		_value(value)
	{
	}
	std::ostream& integer64_node::feed(std::ostream& stream) const
	{
		stream << std::to_string(_value) << std::endl;
		
		return stream;
	}
	
	void integer64_node::set_value(int value)
	{
		_value = value;
	}

	int integer64_node::get_value() const
	{
		return _value;
	}
} }
