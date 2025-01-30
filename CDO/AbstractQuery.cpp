/**
 @file AbstractQuery.hpp
 @date 25.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#include "Query/CreateQuery.hpp"
#include "Query/SelectQuery.hpp"
#include "Query/InsertQuery.hpp"
#include "Query/DeleteQuery.hpp"
#include "Query/DropQuery.hpp"

namespace chaos { namespace cdo {

	abstract_query::~abstract_query() {}

	std::shared_ptr<abstract_query> abstract_query::copy(const abstract_query& src)
	{
		// 1) select ?
		if (auto s = dynamic_cast<const select*>(&src)) {
			return std::make_shared<select>(*s);
		}
		// 2) create ?
		else if (auto cr = dynamic_cast<const create*>(&src)) {
			return std::make_shared<create>(*cr);
		}
		// 3) drop ?
		else if (auto dr = dynamic_cast<const drop*>(&src)) {
			return std::make_shared<drop>(*dr);
		}
		// 4) insert ?
		else if (auto ins = dynamic_cast<const insert*>(&src)) {
			return std::make_shared<insert>(*ins);
		}
		// 5) delete_query ?
		else if (auto delq = dynamic_cast<const delete_query*>(&src)) {
			return std::make_shared<delete_query>(*delq);
		}
		// ... если есть update etc....

		return nullptr;
	}

	void abstract_query::add_cte(const abstract_query& anchor, const std::string& alias)
	{
		_with_queries.push_back({alias, false, copy(anchor)});
	}

	void abstract_query::add_cte(const abstract_query& anchor, const abstract_query& recursive, const std::string& alias, QueryUnionType type)
	{
		_with_queries.push_back({alias, true, copy(anchor), copy(recursive), type});
	}


}}
