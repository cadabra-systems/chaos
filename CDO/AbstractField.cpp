/**
 @file AbstractField.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "AbstractField.hpp"

#include <stdexcept>

namespace chaos { namespace cdo {
	abstract_field::abstract_field(const std::string& name, bool nullable)
	:
		_name(name),
		_nullable(nullable)
	{
		if(_name.empty()) {
			throw std::invalid_argument("Field name cannot be empty!");
		}
	}

	const std::string& abstract_field::get_name() const
	{
		return _name;
	}

	const bool abstract_field::is_nullable() const
	{
		return _nullable;
	}
} }
