/**
 @file Table.cpp
 @date 01.05.2022
 @copyright Cadabra / Ace
 @author daniil@megrabyan.pro
*/

#include "Table.hpp"

#include <cassert>

namespace chaos { namespace cdo {
	table::table(const std::string& name)
	:
	  row_set(name)
	{

	}

	const std::vector<std::shared_ptr<abstract_field>>& table::get_fields () const
	{
		return _fields;
	}

	void table::add_field(std::shared_ptr<abstract_field> field)
	{
		if(!field) {
			throw std::invalid_argument("Name cannot be empty!");
		}

		_fields.push_back(field);
	}

} }
