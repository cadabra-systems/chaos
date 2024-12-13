/**
 @file Redis.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Redis_hpp
#define Chaos_Redis_Redis_hpp

#include <hiredis/hiredis.h>

namespace chaos { namespace redis {
	enum class list_ending : char
	{
		left = 'L',
		right = 'R'
	};

	enum class set_mode : char
	{
		undefined = '0',
		update = 'X', // XX
		create = 'N' // NX
	};
} }

#endif
