/**
 @file Table.cpp
 @date 01.05.2022
 @copyright Cadabra / Ace
 @author daniil@megrabyan.pro
*/

#include "Table.hpp"

namespace chaos { namespace cdo {
	table::table()
	{

	}

	table::~table()
	{
		/// @todo cleanup set
	}

	const integer_field& table::field_integer(const std::string& name, bool nullable, std::int32_t value)
	{
		std::unique_ptr<integer_field> retval(new integer_field(name, nullable, value));

//		_field_set.emplace()

		return *(retval.release());
	}
/*
	const biginteger_field& field_biginteger(const std::string& name, bool nullable = true, std::int64_t value = 0)
	{

	}

	const string_field& field_string(const std::string& name, std::size_t length = 0, bool nullable = true, const std::string& value = "")
	{

	}
*/
} }
