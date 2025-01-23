/**
 @file DropQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "DropQuery.hpp"

namespace chaos { namespace cdo {
	drop::drop(const std::string& tableName, bool ifExists)
	:
	  abstract_query(),
	  _table_name(tableName),
	  _if_exists(ifExists)
	{
	}

	drop::drop(const table& table, bool ifExists)
	:
	  abstract_query(),
	  _table_name(table.name()),
	  _if_exists(ifExists)
	{
	}

	bool drop::operator==(const drop& other) const {
		return
			_table_name == other.table_name() &&
			_if_exists == other.use_if_exists() &&
			_with_queries == other.with_queries();
	}

	drop& drop::if_exists(bool ifExists)
	{
		_if_exists = ifExists;
		return *this;
	}

}}
