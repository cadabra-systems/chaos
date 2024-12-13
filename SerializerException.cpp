/**
 @file SerializerException.cpp
 @date 02.04.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "SerializerException.hpp"

namespace chaos {
	serializer_exception::serializer_exception(const serializer_exception::code code)
	:
		_code(code)
	{
		
	}
	
	const serializer_exception::code& serializer_exception::reason() const
	{
		return _code;
	}
}
