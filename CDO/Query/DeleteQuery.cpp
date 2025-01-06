/**
 @file DeleteQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "DeleteQuery.hpp"
#include <stdexcept>

namespace chaos { namespace cdo {
	drop::drop(const std::string& tableName, bool ifExists)
	:
	  abstract_query(),
	  _table_name(tableName),
	  _if_exists(ifExists)
	{
		if(tableName.empty()) {
			throw std::invalid_argument("table name to DELETE CANNOT be empty!");
		}
	}

	drop::drop(const table& table, bool ifExists)
	:
	  abstract_query(),
	  _table_name(table.name()),
	  _if_exists(ifExists)
	{
		if(table.name().empty()) {
			throw std::invalid_argument("table name to DELETE CANNOT be empty!");
		}
	}

	drop& drop::if_exists(bool ifExists)
	{
		_if_exists = ifExists;
		return *this;
	}

}}
