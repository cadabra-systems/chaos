/**
 @file BooleanBode.cpp
 @date 22.09.15
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "BooleanNode.hpp"

namespace chaos { namespace cxml {
	boolean_node::boolean_node(const bool value)
	:
		_value(value)
	{
	}
	std::ostream& boolean_node::feed(std::ostream& stream) const
	{
		stream << (_value ? "true" : "false") << std::endl;
		
		return stream;
	}
	
	void boolean_node::set_value(bool value)
	{
		_value = value;
	}
	
	bool boolean_node::get_value() const
	{
		return _value;
	}
} }
