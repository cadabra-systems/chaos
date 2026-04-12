/**
 @file IntegerField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
*/

#include "IntegerField.hpp"

namespace chaos { namespace cdo {
	integer_field::integer_field(const std::string& name, bool nullable, std::int32_t value)
	:
		abstract_field(name, nullable),

		_value(value)
	{

	}
} }
