/**
 @file InsertQuery.cpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/


#include "InsertQuery.hpp"

#include <stdexcept>

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
				if(field->get_name().empty()) {
					throw std::logic_error("cannot add empty field to construct INSERT query!");
				}

				_insert_into.push_back(field->get_name());
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

	insert& insert::with(const abstract_query& query)
	{
		auto obj = std::make_shared<insert>(dynamic_cast<const insert&>(query));
		if(!obj) {
			throw std::invalid_argument("WITH statement cannot be empty!");
		}
		_with_queries.push_back({obj, ""});
		return *this;
	}

	insert& insert::columns(const std::vector<std::string>& cols)
	{
		if(cols.empty()){
			throw std::invalid_argument("cannot insert empty data in INSERT query!");
		}

		for(const auto& field : cols) {
			if(field.empty()) {
				throw std::logic_error("cannot add empty field to construct INSERT query!");
			}
			_insert_into.push_back(field);
		}

		return *this;
	}

	insert& insert::columns(std::initializer_list<std::string> cols) {
		if(cols.size() == 0){
			throw std::invalid_argument("cannot insert INTO empty data in INSERT query!");
		}

		_insert_into.insert(_insert_into.end(), cols.begin(), cols.end());
		return *this;
	}

	insert& insert::values(const RowType& row)
	{
		if(row.empty()){
			throw std::invalid_argument("nothing to insert when constructing INSERT query!");
		}

		_rows.push_back(row);
		return *this;
	}

	insert& insert::values(std::initializer_list<InsertValue> row)
	{
		if(row.size() == 0){
			throw std::invalid_argument("nothing to insert when constructing INSERT query!");
		}

		RowType tmp(row.begin(), row.end());
		_rows.push_back(tmp);
		return *this;

	}

	insert& insert::values(std::initializer_list<RowType> rows)
	{
		if(rows.size() == 0){
			throw std::invalid_argument("nothing to insert when constructing INSERT query!");
		}

		for(const auto& row: rows)
		{
			if(row.empty())
				throw std::logic_error("nothing to insert when constructing INSERT query!");

			_rows.push_back(row);
		}

		return *this;
	}

	insert& insert::values(const std::vector<RowType>& rows)
	{
		if(rows.empty()){
			throw std::invalid_argument("nothing to insert when constructing INSERT query!");
		}

		for(const auto& row: rows) {
			if(row.empty()){
				throw std::logic_error("nothing to insert when constructing INSERT query!");
			}
			_rows.push_back(row);
		}
		return *this;
	}

	insert& insert::returning(std::shared_ptr<abstract_field> field)
	{
		if(!field) {
			throw std::invalid_argument("returning field CANNOT be empty!");
		}

		_returning.push_back(field->get_name());
		return *this;
	}

	insert& insert::returning(const std::vector<std::shared_ptr<abstract_field>>& fields)
	{
		if(fields.empty()) {
			throw std::invalid_argument("returning fields CANNOT be empty!");
		}

		for(auto& field: fields) {
			if(!field) {
				throw std::logic_error("returning field CANNOT be empty!");
			}
			_returning.push_back(field->get_name());
		}
		return *this;
	}

	insert& insert::returning(std::initializer_list<std::shared_ptr<abstract_field>> fields)
	{
		if(fields.size() == 0) {
			throw std::invalid_argument("returning fields CANNOT be empty!");
		}

		for(auto& field: fields) {
			if(!field) {
				throw std::logic_error("returning field CANNOT be empty!");
			}
			_returning.push_back(field->get_name());
		}
		return *this;
	}

	insert& insert::returning(const std::string& field)
	{
		if(field.empty()) {
			throw std::invalid_argument("returning field CANNOT be empty!");
		}

		_returning.push_back(field);
		return *this;
	}

	insert& insert::returning(const std::vector<std::string>& fields)
	{
		if(fields.empty()) {
			throw std::invalid_argument("returning fields CANNOT be empty!");
		}

		for(const auto &field: fields) {
			if(field.empty()) {
				throw std::logic_error("returning field CANNOT be empty!");
			}
			_returning.push_back(field);
		}
		return *this;
	}

	insert& insert::returning(std::initializer_list<std::string> fields)
	{
		if(fields.size() == 0) {
			throw std::invalid_argument("returning fields CANNOT be empty!");
		}

		for(const auto &field: fields) {
			if(field.empty()) {
				throw std::logic_error("returning field CANNOT be empty!");
			}
			_returning.push_back(field);
		}
		return *this;
	}

}}
