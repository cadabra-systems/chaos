/**
 @file AbstractField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "AbstractField.hpp"

namespace chaos { namespace cdo {
	abstract_field::abstract_field(const std::string& name, bool nullable)
	:
		_name(name),
		_nullable(nullable)
	{

	}

	const std::string& abstract_field::get_name() const
	{
		return _name;
	}
} }
