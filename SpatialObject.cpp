/**
 @file SpatialObject.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "SpatialObject.hpp"

namespace chaos {
	spatial_object::spatial_object(spatial_object::type type, spatial_object::system system)
	:
		_type(type),
		_system(system)
	{
		
	}
	
	const spatial_object::type& spatial_object::get_type() const
	{
		return _type;
	}
	
	const spatial_object::system& spatial_object::get_system() const
	{
		return _system;
	}
}
