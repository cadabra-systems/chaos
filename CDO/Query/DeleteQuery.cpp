/**
 @file DeleteQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "DeleteQuery.hpp"

namespace chaos { namespace cdo {
	delete_query::delete_query(const std::string& tableName, bool ifExists)
	:
	  abstract_query(),
	  _table_name(tableName),
	  _if_exists(ifExists)
	{
	}

	delete_query::delete_query(const table& table, bool ifExists)
	:
	  abstract_query(),
	  _table_name(table.name()),
	  _if_exists(ifExists)
	{
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

		_returning.push_back(field->get_name());
		return *this;
	}

	delete_query& delete_query::returning(const std::vector<std::shared_ptr<abstract_field>>& fields)
	{

		for(auto& field: fields) {
			_returning.push_back(field->get_name());
		}
		return *this;
	}

	delete_query& delete_query::returning(std::initializer_list<std::shared_ptr<abstract_field>> fields)
	{
		for(auto& field: fields) {
			_returning.push_back(field->get_name());
		}
		return *this;
	}

	delete_query& delete_query::returning(const std::string& field)
	{
		_returning.push_back(field);
		return *this;
	}

	delete_query& delete_query::returning(const std::vector<std::string>& fields)
	{

		for(const auto &field: fields) {
			_returning.push_back(field);
		}
		return *this;
	}

	delete_query& delete_query::returning(std::initializer_list<std::string> fields)
	{

		for(const auto &field: fields) {
			_returning.push_back(field);
		}
		return *this;
	}

}}
