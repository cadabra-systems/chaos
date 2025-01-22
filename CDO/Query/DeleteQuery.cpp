/**
 @file DeleteQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "DeleteQuery.hpp"
#include <stdexcept>

namespace chaos { namespace cdo {
	delete_query::delete_query(const std::string& tableName, bool ifExists)
	:
	  abstract_query(),
	  _table_name(tableName),
	  _if_exists(ifExists)
	{
		if(tableName.empty()) {
			throw std::invalid_argument("table name to DELETE CANNOT be empty!");
		}
	}

	delete_query::delete_query(const table& table, bool ifExists)
	:
	  abstract_query(),
	  _table_name(table.name()),
	  _if_exists(ifExists)
	{
		if(table.name().empty()) {
			throw std::invalid_argument("table name to DELETE CANNOT be empty!");
		}
	}

	bool delete_query::operator==(const delete_query& other) const {
		return
			_table_name == other.table_name() &&
			_if_exists == other.use_if_exists() &&
			_with_queries == other.with_queries();
	}

	delete_query& delete_query::if_exists(bool ifExists)
	{
		_if_exists = ifExists;
		return *this;
	}

	delete_query& delete_query::with(const abstract_query& anchor, const std::string& alias)
	{
		add_cte(anchor, alias);
		return *this;
	}

	delete_query& delete_query::with(const abstract_query& anchor, const abstract_query& reqursive, const std::string& alias, QueryUnionType type)
	{
		add_cte(anchor, reqursive, alias, type);
		return *this;
	}

	delete_query& delete_query::where(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		add_where_condition({ left, op, rightVal });
		return *this;
	}

	delete_query& delete_query::where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		add_where_condition({ left, op, rightVal });
		return *this;
	}

	delete_query& delete_query::where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		add_where_condition({ left, op, rightVal });
		return *this;
	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		return where(left, op, rightVal);
	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		return where(left, op, rightVal);
	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		return where(left, op, rightVal);
	}

	delete_query& delete_query::returning(std::shared_ptr<abstract_field> field)
	{
		if(!field) {
			throw std::invalid_argument("returning field CANNOT be empty!");
		}

		_returning.push_back(field->get_name());
		return *this;
	}

	delete_query& delete_query::returning(const std::vector<std::shared_ptr<abstract_field>>& fields)
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

	delete_query& delete_query::returning(std::initializer_list<std::shared_ptr<abstract_field>> fields)
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

	delete_query& delete_query::returning(const std::string& field)
	{
		if(field.empty()) {
			throw std::invalid_argument("returning field CANNOT be empty!");
		}

		_returning.push_back(field);
		return *this;
	}

	delete_query& delete_query::returning(const std::vector<std::string>& fields)
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

	delete_query& delete_query::returning(std::initializer_list<std::string> fields)
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
