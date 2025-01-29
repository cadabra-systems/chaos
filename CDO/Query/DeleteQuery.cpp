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

	delete_query& delete_query::as(const std::string& name)
	{
		_name = name;
		return *this;
	}

	delete_query& delete_query::asAlias(const std::string& alias)
	{
		_alias = alias;
		return *this;
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
		add_where_condition({left, op, rightVal});
		return *this;
	}

	delete_query& delete_query::where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		add_where_condition({left, op, rightVal});
		return *this;
	}

	delete_query& delete_query::where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		add_where_condition({left, op, rightVal});
		return *this;
	}

	delete_query& delete_query::where(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal)
	{
		add_where_condition({left, op, copy(rightVal)});
		return *this;
	}

	delete_query& delete_query::where(const abstract_query& left, ECompareOp op, const abstract_query& rightVal)
	{
		add_where_condition({copy(left), op, copy(rightVal)});
		return *this;
	}

	delete_query& delete_query::where(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		add_where_condition({copy(left), op, rightVal});
		return *this;
	}

	delete_query& delete_query::where(const abstract_query& left, ECompareOp op, const std::string& rightVal)
	{
		add_where_condition({copy(left), op, rightVal});
		return *this;
	}

	delete_query& delete_query::where(const abstract_query& left, ECompareOp op, int rightVal)
	{
		add_where_condition({copy(left), op, rightVal});
		return *this;
	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;

	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::and_(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::and_(const abstract_query& left, ECompareOp op, const abstract_query& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::and_(const abstract_query& left, ECompareOp op, int rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::and_(const abstract_query& left, ECompareOp op, const std::string& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::and_(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::AND_;
		return *this;
	}

	delete_query& delete_query::or_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::or_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;

	}

	delete_query& delete_query::or_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::or_(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::or_(const abstract_query& left, ECompareOp op, const abstract_query& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::or_(const abstract_query& left, ECompareOp op, int rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::or_(const abstract_query& left, ECompareOp op, const std::string& rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::or_(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		where(left, op, rightVal);
		_where_conditions.back().logicOp = abstract_query::ELogicOp::OR_;
		return *this;
	}

	delete_query& delete_query::using_(std::shared_ptr<abstract_query> query)
	{
		_using.push_back(query);
		return *this;
	}

	delete_query& delete_query::using_(const abstract_query& query)
	{
		_using.push_back(copy(query));
		return *this;
	}

	delete_query& delete_query::using_(std::shared_ptr<table> query)
	{
		_using.push_back(query);
		return *this;
	}

	delete_query& delete_query::using_(const table& query)
	{
		_using.push_back(std::make_shared<table>(query));
		return *this;
	}

	delete_query& delete_query::using_(std::shared_ptr<view> query)
	{
		_using.push_back(query);
		return *this;
	}

	delete_query& delete_query::using_(const view& query)
	{
		_using.push_back(std::make_shared<view>(query));
		return *this;
	}

	delete_query& delete_query::using_(const std::vector<std::shared_ptr<abstract_query>>& queries)
	{
		_using.insert(_using.end(), queries.begin(), queries.end());
		return *this;
	}

	delete_query& delete_query::using_(const std::vector<abstract_query>& queries)
	{
		for(const auto& q: queries){
			_using.push_back(copy(q));
		}
		return *this;
	}

	delete_query& delete_query::using_(const std::vector<std::shared_ptr<table>>& queries)
	{
		_using.insert(_using.end(), queries.begin(), queries.end());
		return *this;
	}

	delete_query& delete_query::using_(const std::vector<table>& queries)
	{
		for(const auto& q: queries){
			_using.push_back(std::make_shared<table>(q));
		}
		return *this;
	}

	delete_query& delete_query::using_(const std::vector<std::shared_ptr<view>>& queries)
	{
		_using.insert(_using.end(), queries.begin(), queries.end());
		return *this;
	}

	delete_query& delete_query::using_(const std::vector<view>& queries)
	{
		for(const auto& q: queries){
			_using.push_back(std::make_shared<view>(q));
		}
		return *this;
	}

	delete_query& delete_query::using_(std::initializer_list<std::shared_ptr<abstract_query>> queries)
	{
		_using.insert(_using.end(), queries.begin(), queries.end());
		return *this;
	}

	delete_query& delete_query::using_(std::initializer_list<abstract_query> queries)
	{
		for(const auto& q: queries){
			_using.push_back(copy(q));
		}
		return *this;
	}

	delete_query& delete_query::using_(std::initializer_list<std::shared_ptr<table>> queries)
	{
		_using.insert(_using.end(), queries.begin(), queries.end());
		return *this;
	}

	delete_query& delete_query::using_(std::initializer_list<table> queries)
	{
		for(const auto& q: queries){
			_using.push_back(std::make_shared<table>(q));
		}
		return *this;
	}

	delete_query& delete_query::using_(std::initializer_list<std::shared_ptr<view>> queries)
	{
		_using.insert(_using.end(), queries.begin(), queries.end());
		return *this;
	}

	delete_query& delete_query::using_(std::initializer_list<view> queries)
	{
		for(const auto& q: queries){
			_using.push_back(std::make_shared<view>(q));
		}
		return *this;
	}

	delete_query& delete_query::returning(std::shared_ptr<abstract_field> field)
	{

		_returning.push_back(field->name());
		return *this;
	}

	delete_query& delete_query::returning(const std::vector<std::shared_ptr<abstract_field>>& fields)
	{

		for(auto& field: fields) {
			_returning.push_back(field->name());
		}
		return *this;
	}

	delete_query& delete_query::returning(std::initializer_list<std::shared_ptr<abstract_field>> fields)
	{
		for(auto& field: fields) {
			_returning.push_back(field->name());
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
