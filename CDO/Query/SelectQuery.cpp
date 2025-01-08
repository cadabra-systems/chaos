/**
 @file SelectQuery.cpp
 @date 25.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#include "SelectQuery.hpp"
#include <stdexcept>

namespace chaos { namespace cdo {

bool select::operator==(const select& other) const {
		return
			_with_queries == other.with_queries() &&
			_selectable_fields == other.selectable_fields() &&
			_from_tables == other.from_tables() &&
			_from_subqueries == other.from_subqueries() &&
			_joins == other.joins() &&
			_where_conditions == other.where_conditions() &&
			_group_by_fields == other.groupBy() &&
			_order_by == other.orderBy();
	}

	select& select::with(const abstract_query& cte)
	{
		auto obj = std::make_shared<select>(dynamic_cast<const select&>(cte));
		if(!obj) {
			throw std::invalid_argument("WITH statement cannot be empty!");
		}
		_with_queries.push_back(obj);
		return *this;
	}

	select& select::fields(std::initializer_list<std::shared_ptr<abstract_field>> fields)
	{
		if(fields.size() == 0) {
			throw std::invalid_argument("selectable fields cannot be empty!");
		}
		_selectable_fields.insert(_selectable_fields.end(), fields.begin(), fields.end());
		return *this;
	}

	select& select::fields(const std::vector<std::shared_ptr<abstract_field>>& fields)
	{
		if(fields.empty()) {
			throw std::invalid_argument("selectable fields cannot be empty!");
		}

		for(const auto& field: fields) {
			if(!field) {
				throw std::invalid_argument("selectable field cannot be empty!");
			}
			_selectable_fields.push_back(field);
		}

		return *this;
	}

	select& select::fields(std::shared_ptr<abstract_field> field)
	{
		if(!field) {
			throw std::invalid_argument("selectable field cannot be empty!");
		}

		_selectable_fields.push_back(field);
		return *this;
	}


	select& select::from(const table& t)
	{
		if(t.get_fields().empty()) {
			throw std::invalid_argument("argument table is empty!");
		}

		_from_tables.push_back(std::make_shared<table>(t));
		return *this;
	}

	select& select::from(const view& v)
	{
		if(v.get_fields().empty()) {
			throw std::invalid_argument("argument table is empty!");
		}

		_from_tables.push_back(std::make_shared<view>(v));
		return *this;
	}

	select& select::from(const abstract_query& subq)
	{
		auto obj = std::make_shared<select>(dynamic_cast<const select&>(subq));
		if(!obj) {
			throw std::invalid_argument("from statement cannot be empty!");
		}

		_from_subqueries.push_back(obj);
		return *this;
	}

	select& select::where(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		add_where_condition({left, op, rightVal});
		return *this;
	}

	select& select::where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		add_where_condition({left, op, rightVal});
		return *this;
	}

	select& select::where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		add_where_condition({left, op, rightVal});
		return *this;
	}

	select& select::and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal)
	{
		return where(left, op, rightVal);
	}

	select& select::and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal)
	{
		return where(left, op, rightVal);
	}

	select& select::and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal)
	{
		return where(left, op, rightVal);
	}

	select& select::join(const row_set& rs, EJoinType type)
	{
		if (rs.get_fields().empty()) {
			throw std::invalid_argument("join(...) - row_set is empty");
		}

		try {
			auto& t = dynamic_cast<const table&>(rs);
			_joins.push_back( JoinInfo {
				std::make_shared<table>(t),
				type,
				{}
			});
		}
		catch(const std::bad_cast&) {
			auto& v = dynamic_cast<const view&>(rs);
			_joins.push_back( JoinInfo {
				std::make_shared<view>(v),
				type,
				{}
			});
		}
		return *this;
	}

	select& select::on(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> right)
	{
		if (_joins.empty()) {
			throw std::logic_error("on(...) called but no JOIN is defined");
		}

		if (!left || !right) {
			throw std::invalid_argument("on(...) - one of fields is null");
		}

		auto& lastJoin = _joins.back();
		lastJoin.on_conditions.push_back( Condition { left, op, right } );
		return *this;
	}

	select& select::order(std::shared_ptr<abstract_field> field, bool ascending)
	{
		_order_by = field->get_name() + (ascending ? " ASC" : " DESC");
		return *this;
	}

	select& select::group(std::shared_ptr<abstract_field> field)
	{
		_group_by_fields.push_back(field);
		return *this;
	}

	select& select::join_inner(const row_set& rs)   { return join(rs, EJoinType::Inner); }
	select& select::join_left(const row_set& rs)    { return join(rs, EJoinType::Left); }
	select& select::join_right(const row_set& rs)   { return join(rs, EJoinType::Right); }
	select& select::join_full(const row_set& rs)    { return join(rs, EJoinType::Full); }
	select& select::join_cross(const row_set& rs)   { return join(rs, EJoinType::Cross); }
	select& select::join_natural(const row_set& rs) { return join(rs, EJoinType::Natural); }

	select& select::join_left_exclusion(const row_set& rs)  { return join(rs, EJoinType::LeftExclusion); }
	select& select::join_right_exclusion(const row_set& rs) { return join(rs, EJoinType::RightExclusion); }
	select& select::join_full_exclusion(const row_set& rs)  { return join(rs, EJoinType::FullExclusion); }
	select& select::join_self(const row_set& rs)            { return join(rs, EJoinType::SelfJoin); }

}}
