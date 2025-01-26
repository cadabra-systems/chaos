#include "PostgreSQL.hpp"

#include "../Field/IntegerField.hpp"
#include "../Field/BigSignedInteger.hpp"
#include "../Field/String.hpp"

#include <sstream>
#include <stdexcept>

namespace chaos { namespace cdo {

	std::string postgresql::escape_string(const std::string& input) const
	{
		std::string result;
		result.reserve(input.size() * 2);

		for (char c : input) {
			if (c == '\'') {
				result += "''";
			} else {
				result.push_back(c);
			}
		}
		return result;
	}

	bool postgresql::isLiteral(
		const std::variant<
			std::shared_ptr<abstract_field>,
			std::shared_ptr<row_set>,
			std::shared_ptr<abstract_query>,
			int, std::string
		>& v
	) const
	{
		return std::holds_alternative<int>(v) || std::holds_alternative<std::string>(v);
	}

	void postgresql::printName(
		std::ostream& out,
		const std::variant<
			std::shared_ptr<abstract_field>,
			std::shared_ptr<row_set>,
			std::shared_ptr<abstract_query>,
			int, std::string
		>& v
	) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {
				if (!val) {
					throw std::logic_error("Null field in printName()");
				}
				out << val->get_name();
			}
			else if constexpr (std::is_same_v<T, std::shared_ptr<abstract_query>>) {
				// Если subquery имеет alias, печатаем alias.
				out << val->alias();
			}
			else if constexpr (std::is_same_v<T, std::shared_ptr<row_set>>) {
				out << val->name();
			}
			else if constexpr (std::is_same_v<T, row_set>) {
				out << val.name();
			}

		}, v);
	}

	void postgresql::printValue(
		std::ostream& out,
		const std::variant<
			std::shared_ptr<abstract_field>,
			std::shared_ptr<row_set>,
			std::shared_ptr<abstract_query>,
			int, std::string
		>& v
	) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;

			// 1) Если это field => печатаем _value (по старой логике)
			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {
				if(!val) {
					throw std::logic_error("Field is null in printValue(...)");
				}

				if(auto big = std::dynamic_pointer_cast<big_signed_integer>(val)) {
					out << big->get_value();
					return;
				}
				if(auto si = std::dynamic_pointer_cast<signed_integer>(val)) {
					out << si->get_value();
					return;
				}
				if(auto s = std::dynamic_pointer_cast<string>(val)) {
					// Если хотим поддержать isRaw():
					//   if(s->isRaw()) { out << s->get_value(); } else ...
					auto safe = escape_string(s->get_value());
					out << "'" << safe << "'";
					return;
				}
				// если есть другие классы-наследники abstract_field, обрабатывайте их
				throw std::runtime_error("Unsupported field type in printValue()");
			}

			// 2) Если подзапрос => (SELECT ...)
			else if constexpr (std::is_same_v<T, std::shared_ptr<abstract_query>>) {
				// Для INSERT ... VALUES( (SELECT...) )?
				// В теории возможно. Тогда печатаем "(" + query + ")".
				if(!val->alias().empty()) {
					out << val->alias();
				} else {
					out << "(" << processQuery(*val, true) << ")";
				}
			}

			// 3) int => число без кавычек
			else if constexpr (std::is_same_v<T, int>) {
				out << val;
			}

			// 4) std::string => строковый литерал
			else if constexpr (std::is_same_v<T, std::string>) {
				auto safe = escape_string(val);
				out << "'" << safe << "'";
			}
		}, v);
	}

	std::string postgresql::generateReturning(const std::vector<std::string>& returning) const
	{
		std::ostringstream out;
		if(!returning.empty()) {
			out << " RETURNING ";
			bool first = true;
			for(const auto& r : returning) {
				if(!first) { out << ", "; }
				out << r;
				first = false;
			}
		}
		return out.str();
	}

	std::string postgresql::generateWhere(const std::vector<abstract_query::Condition>& whereConditions) const
	{
		if(whereConditions.empty()) {
			return "";
		}

		std::ostringstream out;
		out << " WHERE (";
		bool firstCondition = true;

		for (const auto &cond : whereConditions) {
			if(!firstCondition) {
				out << " AND ";
			}
			firstCondition = false;

			printName(out, cond.left_field);
			out << " " << abstract_query::to_string(cond.op) << " ";

			if(cond.op == abstract_query::ECompareOp::IN ||
			   cond.op == abstract_query::ECompareOp::NOT_IN)
			{
				if(auto subq = std::get_if<std::shared_ptr<abstract_query>>(&cond.right_value)) {
					out << "(" << processQuery(**subq, true) << ")";
				}
				else {
					out << "(/* ??? */)";
				}
			}
			else if(cond.op == abstract_query::ECompareOp::NOT_LIKE ||
					cond.op == abstract_query::ECompareOp::IN ||
					cond.op == abstract_query::ECompareOp::NOT_IN )
			{
				if(std::holds_alternative<std::shared_ptr<abstract_query>>(cond.right_value)) {
					auto subq = std::get<std::shared_ptr<abstract_query>>(cond.right_value);
					out << "(" << processQuery(*subq, true) << ")";
				}
				else if(isLiteral(cond.right_value)) {
					printValue(out, cond.right_value);
				}
				else {
					printName(out, cond.right_value);
				}
			}
			else {
				if(std::holds_alternative<std::shared_ptr<abstract_query>>(cond.right_value)) {
					auto subq = std::get<std::shared_ptr<abstract_query>>(cond.right_value);
					out << "(" << processQuery(*subq, true) << ")";
				}
				else if(isLiteral(cond.right_value)) {
					printValue(out, cond.right_value);
				}
				else {
					printName(out, cond.right_value);
				}
			}
		}

		out << ")";
		return out.str();
	}

	std::string postgresql::generateCTE(const abstract_query& query) const
	{
		std::ostringstream out;
		auto with_queries = query.with_queries();

		if(!with_queries.empty()) {
			for(int i = 0; i < static_cast<int>(with_queries.size()); i++) {
				const auto &w = with_queries[i];

				if(w.anchor->has_modifier(abstract_query::QueryModifiers::RECURSIVE)) {
					out << generateCTE(*w.anchor);
				}
				else {
					auto alias = w.alias.empty() ? ("cte" + std::to_string(i)) : w.alias;
					out << alias << " AS (";
					out << processQuery(*w.anchor, true);
					if(w.is_recursive && w.recursive) {
						out << (w.union_type == abstract_query::QueryUnionType::Union
								? " UNION "
								: " UNION ALL ");
						out << processQuery(*w.recursive, true);
					}
					out << ")";
				}

				if(i+1 < (int)with_queries.size()) {
					out << ", ";
				}
			}
		}
		return out.str();
	}

	std::string	postgresql::generateSelectQuery(const select& query, bool isSubquery) const
	{
		std::ostringstream out;

		auto with_queries = query.with_queries();
		if(!with_queries.empty()) {
			out << "WITH ";
			bool isRec = false;
			for(const auto &c : with_queries) {
				if(c.is_recursive ||
				   c.anchor->has_modifier(abstract_query::QueryModifiers::RECURSIVE))
				{
					isRec = true;
					break;
				}
			}
			if(isRec) {
				out << "RECURSIVE ";
			}
		}

		out << generateCTE(query);

		// SELECT fields...
		auto fields = query.selectable_fields();
		if(!isSubquery) {
			out << " ";
		}
		out << "SELECT ";
		if(query.has_modifier(abstract_query::QueryModifiers::DISTINCT)) {
			out << "DISTINCT ";
		}
		if(!fields.empty()) {
			for(size_t i=0; i<fields.size(); i++) {
				// alias + " AS " + name
				if(!fields[i]->alias().empty()) {
					out << fields[i]->alias() << " AS ";
				}
				out << fields[i]->get_name();
				if(i+1 < fields.size()) {
					out << ", ";
				}
			}
		} else {
			out << "*";
		}

		// FROM ...
		auto from_tables = query.from_tables();
		auto from_subqueries = query.from_subqueries();
		if(!from_tables.empty() || !from_subqueries.empty()) {
			out << " FROM ";
			bool needComma = false;

			for(const auto &tbl : from_tables) {
				if(needComma) out << ", ";
				out << tbl->name();
				needComma = true;
			}
			for(size_t i=0; i<from_subqueries.size(); i++) {
				if(needComma) out << ", ";
				auto subSel = std::dynamic_pointer_cast<select>(from_subqueries[i]);
				if(subSel) {
					auto it = std::find_if(
						with_queries.begin(),
						with_queries.end(),
						[&](auto &cteInfo){
							auto s = std::dynamic_pointer_cast<select>(cteInfo.anchor);
							return s && *s == *subSel;
						}
					);
					if(it != with_queries.end()) {
						size_t idx = std::distance(with_queries.begin(), it);
						auto cteAlias = it->alias.empty() ? ("cte" + std::to_string(idx)) : it->alias;
						out << cteAlias;
					} else {
						out << "(" << generateSelectQuery(*subSel, true) << ") AS sub" << i;
					}
				} else {
					out << "(/* unknown or non-select subquery */) AS sub" << i;
				}
				needComma = true;
			}
		}

		// JOIN ...
		auto joins = query.joins();
		for(const auto &jn : joins) {
			out << " " << abstract_query::to_string(jn.join_type) << " ";
			printName(out, jn.joined_rs);

			if(!jn.on_conditions.empty()) {
				out << " ON ";
				if(jn.on_conditions.size() > 1) out << "(";

				bool firstCond = true;
				for(const auto &cond : jn.on_conditions) {
					if(!firstCond) out << " AND ";
					firstCond = false;

					printName(out, cond.left_field);
					out << " " << abstract_query::to_string(cond.op) << " ";

					if(cond.op == abstract_query::ECompareOp::IN ||
					   cond.op == abstract_query::ECompareOp::NOT_IN)
					{
						if(auto subq = std::get_if<std::shared_ptr<abstract_query>>(&cond.right_value)) {
							out << "(" << processQuery(**subq, true) << ")";
						} else {
							out << "(/* ??? */)";
						}
					}
					else if(isLiteral(cond.right_value)) {
						printValue(out, cond.right_value);
					} else if(std::holds_alternative<std::shared_ptr<abstract_query>>(cond.right_value)) {
						auto subq = std::get<std::shared_ptr<abstract_query>>(cond.right_value);
						out << "(" << processQuery(*subq, true) << ")";
					} else {
						printName(out, cond.right_value);
					}
				}

				if(jn.on_conditions.size() > 1) out << ")";
			}
		}

		// WHERE ...
		out << generateWhere(query.where_conditions());

		// GROUP BY ...
		auto groupBy = query.groupBy();
		if(!groupBy.empty()) {
			out << " GROUP BY ";
			for(size_t i=0; i<groupBy.size(); i++) {
				out << groupBy[i]->get_name();
				if(i+1<groupBy.size()) {
					out << ", ";
				}
			}
		}

		// ORDER BY ...
		auto orderBy = query.orderBy();
		if(!orderBy.empty()) {
			out << " ORDER BY " << orderBy;
		}

		// UNION / UNION ALL
		auto unions = query.unions();
		if(!unions.empty()) {
			for(const auto &u : unions) {
				out << (u.second == abstract_query::QueryUnionType::Union
						? " UNION "
						: " UNION ALL ");
				out << processQuery(*u.first, true);
			}
		}

		if(!isSubquery) {
			out << ";";
		}

		std::string res = out.str();
		if(res.front() == ' ') {
			res.erase(0,1);
		}

		return res;
	}

	std::string postgresql::generateCreateQuery(const create& query) const
	{
		std::ostringstream out;
		out << "CREATE TABLE ";
		if(query.use_if_not_exists()) {
			out << "IF NOT EXISTS ";
		}
		out << query.table_name() << " (";

		const auto &cols = query.columns_list();
		bool first = true;
		for(const auto &c : cols) {
			if(!first) out << ", ";
			first = false;

			out << c->to_SQL();
		}

		const auto &pks = query.primary_keys();
		if(!pks.empty()) {
			out << ", PRIMARY KEY (";
			for(size_t i=0; i<pks.size(); i++){
				out << pks[i];
				if(i+1<pks.size()) out << ", ";
			}
			out << ")";
		}

		const auto &fks = query.foreign_keys();
		for(const auto &fk : fks) {
			out << ", CONSTRAINT " << fk.constraint_name << " FOREIGN KEY (";
			for(size_t i=0; i<fk.columns.size(); i++){
				out << fk.columns[i];
				if(i+1<fk.columns.size()) out << ", ";
			}
			out << ") REFERENCES " << fk.ref_table << "(";
			for(size_t i=0; i<fk.ref_columns.size(); i++){
				out << fk.ref_columns[i];
				if(i+1<fk.ref_columns.size()) out << ", ";
			}
			out << ")";

			if(!fk.on_update.empty()) {
				out << " ON UPDATE " << fk.on_update;
			}
			if(!fk.on_delete.empty()) {
				out << " ON DELETE " << fk.on_delete;
			}
		}

		out << ");";
		return out.str();
	}

	std::string postgresql::generateDeleteQuery(const delete_query &query, bool isSubquery) const
	{
		std::ostringstream out;
		out << generateCTE(query);
		out << "DELETE FROM " << query.table_name();
		out << generateWhere(query.where_conditions());
		out << generateReturning(query.returning_list());

		if(!isSubquery) {
			out << ";";
		}
		return out.str();
	}

	std::string postgresql::generateDropQuery(const drop &query, bool isSubquery) const
	{
		std::ostringstream out;
		out << generateCTE(query);
		out << "DROP TABLE ";
		if(query.use_if_exists()) {
			out << "IF EXISTS ";
		}
		out << query.table_name();
		if(!isSubquery) {
			out << ";";
		}
		return out.str();
	}

	std::string postgresql::generateInsertQuery(const insert& query, bool isSubquery) const
	{
		std::ostringstream out;
		out << generateCTE(query);

		out << "INSERT INTO " << query.table_name();

		const auto &cols = query.columns_list();
		if(cols.empty()) {
			throw std::logic_error("INSERTABLE FIELDS CANNOT BE EMPTY!");
		}

		out << " (";
		for(size_t i=0; i<cols.size(); i++){
			out << cols[i];
			if(i+1 < cols.size()){
				out << ", ";
			}
		}
		out << ")";

		const auto &rows = query.rows();
		if(rows.empty()) {
			out << " DEFAULT VALUES";
			return out.str();
		}

		out << " VALUES ";
		for(size_t r=0; r<rows.size(); r++){
			if(r>0) out << ", ";
			out << "(";
			for(size_t c=0; c<rows[r].size(); c++){
				printValue(out, rows[r][c]);
				if(c+1 < rows[r].size()){
					out << ", ";
				}
			}
			out << ")";
		}

		out << generateReturning(query.returning_list());

		if(!isSubquery){
			out << ";";
		}
		return out.str();
	}

}}
