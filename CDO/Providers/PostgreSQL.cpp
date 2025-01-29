#include "PostgreSQL.hpp"

#include "../Field/IntegerField.hpp"
#include "../Field/BigSignedInteger.hpp"
#include "../Field/String.hpp"

#include <sstream>

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

	bool postgresql::isLiteral(const abstract_query::AbstractVariant& v) const
	{
		return std::holds_alternative<int>(v) || std::holds_alternative<std::string>(v);
	}

	void postgresql::printName(std::ostream& out, const abstract_query::AbstractVariant& v) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {

				if(val->is_rawExression())
					printValue(out, val);
				else out << (val->tableAlias().empty() ? "" : val->tableAlias() + ".") << val->name();
			}
			else if constexpr (std::is_same_v<T, std::shared_ptr<abstract_query>>) {
				if(!val->name().empty())
					out << val->name();
				else out << processQuery(*val, true);
			}
			else if constexpr (std::is_same_v<T, std::shared_ptr<row_set>>) {
				out << val->name();
			}
			else if constexpr (std::is_same_v<T, row_set>) {
				out << val.name();
			}

		}, v);
	}

	void postgresql::printValue(std::ostream& out, const abstract_query::AbstractVariant& v) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {

				if(val->is_rawExression()) {
					out << val->expression();
					return;
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
					auto safe = escape_string(s->get_value());
					out << "'" << safe << "'";
					return;
				}
				return;
			}

			else if constexpr (std::is_same_v<T, std::shared_ptr<abstract_query>>) {
				if(!val->alias().empty()) {
					out << val->alias();
				} else {
					out << "(" << processQuery(*val, true) << ")";
				}
			}

			else if constexpr (std::is_same_v<T, int>) {
				out << val;
			}

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
		out << " WHERE ";

		for (const auto& cond : whereConditions) {

			if (cond != whereConditions.front()) {
				out << " " << abstract_query::to_string(cond.logicOp) << " ";
			}

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

	std::string postgresql::processCTE(const abstract_query& query) const
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

		return out.str();
	}

	std::string postgresql::postProcessOutput(const std::string& out) const
	{
		auto str = out;
		if(str.front() == ' ') {
			str.erase(0,1);
		}
		return str;
	}

	std::string postgresql::processFieldCreation(const abstract_field& field) const
	{
		std::ostringstream out;

		out << field.name();
		switch(field.field_type()){
		case chaos::cdo::abstract_field::fieldType::big_signed_integer:{
			out << " BIGINT";
			break;
		}
		case chaos::cdo::abstract_field::fieldType::signed_integer:{
			out << " INTEGER";
			break;
		}
		case chaos::cdo::abstract_field::fieldType::string:{
			const auto& str_field = dynamic_cast<const chaos::cdo::string&>(field);
			out << " VARCHAR(" << static_cast<int>(str_field.get_length()) << ")";
			break;
		}}

		if(!field.is_nullable()) {
			out << " NOT NULL";
		}

		return out.str();

	}

	std::string	postgresql::generateSelectQuery(const select& query, bool isSubquery) const
	{
		std::ostringstream out;

		auto with_queries = query.with_queries();

		if(!isSubquery){
			out << processCTE(query);
			out << generateCTE(query);
		}

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
			for(size_t i = 0; i < fields.size(); i++) {

				if(fields[i]->is_rawExression()){
					out << fields[i]->expression();
				} else if (!fields[i]->name().empty()) {
					out << (fields[i]->tableAlias().empty() ? "" : fields[i]->tableAlias() + ".") << fields[i]->name();
				}
				else continue;

				if(!fields[i]->alias().empty()) {
					out << " AS " << fields[i]->alias();
				}

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
			for(size_t i=0; i < from_subqueries.size(); i++) {
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
						if(!subSel->name().empty()) {
							out << subSel->name();
						}
						else out << "(" << generateSelectQuery(*subSel, true) << ") AS sub" << i;
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
			}
		}

		// WHERE ...
		out << generateWhere(query.where_conditions());

		// GROUP BY ...
		auto groupBy = query.groupBy();
		if(!groupBy.empty()) {
			out << " GROUP BY ";
			for(size_t i=0; i<groupBy.size(); i++) {
				out << groupBy[i]->name();
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

		return postProcessOutput(out.str());
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

			out << processFieldCreation(*c.get());
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

		if(!isSubquery){
			out << processCTE(query);
			out << generateCTE(query);
		}

		out << " DELETE FROM " << query.table_name();

		const auto& using_queries = query.get_using();
		if (!using_queries.empty()) {
			out << " USING ";
			for (size_t i = 0; i < using_queries.size(); ++i) {
				std::visit([&](auto&& val) {
					using T = std::decay_t<decltype(val)>;
					if constexpr (std::is_same_v<T, std::shared_ptr<abstract_query>>) {
						if(!val->name().empty())
							out << val->name() << " " << val->alias();
						else out << "(" << processQuery(*val, true) << ")" << " " <<val->alias();
					}
					else if constexpr (std::is_base_of_v<std::shared_ptr<row_set>, T>) {
						out << val->name();
					}
				}, using_queries[i]);

				if (i < using_queries.size() - 1) {
					out << ", ";
				}
			}
		}
		out <<generateWhere(query.where_conditions());
		out << generateReturning(query.returning_list());

		if(!isSubquery) {
			out << ";";
		}
		return postProcessOutput(out.str());
	}

	std::string postgresql::generateDropQuery(const drop &query, bool isSubquery) const
	{
		std::ostringstream out;
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

		if(!isSubquery){
			out << processCTE(query);
			out << generateCTE(query);
		}

		if(!isSubquery) {
			out << " ";
		}
		out << "INSERT INTO " << query.table_name();

		const auto &cols = query.columns_list();
		out << " (";
		for(size_t i = 0; i < cols.size(); i++){
			out << cols[i]->name();
			if(i + 1 < cols.size()){
				out << ", ";
			}
		}
		out << ")";

		// Handle SELECT-based insertion
		if (query.selectable()) {
			out << " " << generateSelectQuery(*query.selectable(), true);
		} else {
			// Handle VALUES-based insertion
			const auto &rows = query.rows();
			if (rows.empty()) {
				out << " DEFAULT VALUES";
			} else {
				out << " VALUES ";
				for (size_t r = 0; r < rows.size(); ++r) {
					if (r > 0) out << ", ";
					out << "(";
					for (size_t c = 0; c < rows[r].size(); ++c) {
						printValue(out, rows[r][c]);
						if (c + 1 < rows[r].size()) {
							out << ", ";
						}
					}
					out << ")";
				}
			}
		}

		// Add RETURNING clause if needed
		out << generateReturning(query.returning_list());

		if (!isSubquery) {
			out << ";";
		}

		return postProcessOutput(out.str());
	}

}}
