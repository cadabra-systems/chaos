/**
 @file InsertQuery.cpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/


#include "InsertQuery.hpp"

namespace chaos { namespace cdo {

	insert::insert(const std::string& tableName)
	:
	  abstract_query(),
	  _table_name(tableName)
	{
		if(tableName.empty()){
			throw std::invalid_argument("table name in INSERT query CANNOT be empty!");
		}
	}

	insert::insert(const table& table, bool useAllFields)
	:
	  abstract_query(),
	  _table_name(table.name()),
	  _use_all_fields(useAllFields)
	{
		if(table.name().empty()){
			throw std::invalid_argument("table name in INSERT query CANNOT be empty!");
		}

		if(useAllFields)
		{
			if(table.get_fields().empty()){
				throw std::logic_error("table contents in INSERT query CANNOT be empty, if you want initialize it on startup!");
			}

			for(const auto& field : table.get_fields()) {
				if(field->name().empty()) {
					throw std::logic_error("cannot add empty field to construct INSERT query!");
				}

				_insert_into.push_back(field->name());
			}
		}
	}

	bool insert::operator==(const insert& other) const
	{
		return
			_table_name == other.table_name() &&
			_use_all_fields == other._use_all_fields;
			_insert_into == other.columns_list() &&
			_rows == other.rows() &&
			_with_queries == other.with_queries();
	}

	insert& insert::with(const abstract_query& anchor, const std::string& alias)
	{
		add_cte(anchor, alias);
		return *this;
	}

	insert& insert::with(const abstract_query& anchor, const abstract_query& reqursive, const std::string& alias, QueryUnionType type)
	{
		add_cte(anchor, reqursive, alias, type);
		return *this;
	}

	insert& insert::columns(const std::vector<std::string>& cols)
	{
		for(const auto& field : cols) {
			_insert_into.push_back(field);
		}

		return *this;
	}

	insert& insert::columns(std::initializer_list<std::string> cols) {
		_insert_into.insert(_insert_into.end(), cols.begin(), cols.end());
		return *this;
	}

	insert& insert::values(const RowType& row)
	{
		_rows.push_back(row);
		return *this;
	}

	insert& insert::values(std::initializer_list<InsertValue> row)
	{
		RowType tmp(row.begin(), row.end());
		_rows.push_back(tmp);
		return *this;

	}

	insert& insert::values(std::initializer_list<RowType> rows)
	{
		for(const auto& row: rows) {
			_rows.push_back(row);
		}

		return *this;
	}

	insert& insert::values(const std::vector<RowType>& rows)
	{
		for(const auto& row: rows) {
			_rows.push_back(row);
		}
		return *this;
	}

	insert& insert::returning(std::shared_ptr<abstract_field> field)
	{
		_returning.push_back(field->name());
		return *this;
	}

	insert& insert::returning(const std::vector<std::shared_ptr<abstract_field>>& fields)
	{
		for(auto& field: fields) {
			_returning.push_back(field->name());
		}
		return *this;
	}

	insert& insert::returning(std::initializer_list<std::shared_ptr<abstract_field>> fields)
	{
		for(auto& field: fields) {
			_returning.push_back(field->name());
		}
		return *this;
	}

	insert& insert::returning(const std::string& field)
	{
		_returning.push_back(field);
		return *this;
	}

	insert& insert::returning(const std::vector<std::string>& fields)
	{
		for(const auto &field: fields) {
			_returning.push_back(field);
		}
		return *this;
	}

	insert& insert::returning(std::initializer_list<std::string> fields)
	{
		for(const auto &field: fields) {
			_returning.push_back(field);
		}
		return *this;
	}

}}
