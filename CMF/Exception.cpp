/**
 @file Exception.cpp
 @date 02.04.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Exception.hpp"

namespace chaos { namespace cmf {
	exception::exception(exception::code code)
	:
		_code(code)
	{
		
	}
	
	const exception::code& exception::reason() const
	{
		return _code;
	}
} }
