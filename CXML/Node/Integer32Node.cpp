/**
 @file Integer32Node.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Integer32Node.hpp"

namespace chaos { namespace cxml {
	integer32_node::integer32_node(const std::int32_t value)
	:
		_value(value)
	{
	}
	std::ostream& integer32_node::feed(std::ostream& stream) const
	{
		stream << std::to_string(_value) << std::endl;
		
		return stream;
	}
	
	void integer32_node::set_value(int value)
	{
		_value = value;
	}

	int integer32_node::get_value() const
	{
		return _value;
	}
} }
