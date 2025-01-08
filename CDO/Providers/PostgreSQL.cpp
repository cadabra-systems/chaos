/**
 @file PosgtreSQL.cpp
 @date 28.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

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

	void postgresql::printValue(std::ostream& out, const std::variant<std::shared_ptr<abstract_field>, int, std::string>& v) const
	{
		std::visit([&](auto&& val) {
			using T = std::decay_t<decltype(val)>;
			if constexpr (std::is_same_v<T, std::shared_ptr<abstract_field>>) {
				if (!val) {
					out << "NULL_FIELD";
					return;
				}

				if (auto big = std::dynamic_pointer_cast<big_signed_integer>(val)) {
					out << big->get_value();
				}

				if (auto si = std::dynamic_pointer_cast<signed_integer>(val)) {
					out << si->get_value();
				}

				if (auto str = std::dynamic_pointer_cast<string>(val)) {
					auto safe = escape_string(str->get_value());
					out << "'" << safe << "'";
				}
			}

			if constexpr (std::is_same_v<T, int>) {
				out << val;
			}

			if constexpr (std::is_same_v<T, std::string>) {
				out << "'" << val << "'";
			}
		}, v);
	}

    std::string	postgresql::generateSelectQuery(const select& query, bool isSubquery) const
	{
		std::ostringstream out;

		auto with_queries = query.with_queries();
		if(!with_queries.empty()) {
			out << "WITH ";
			for (size_t i = 0; i < with_queries.size(); ++i) {

				auto subSel = std::dynamic_pointer_cast<select>(with_queries[i]);
				if (!subSel) {
					out << "cte" << i << " AS (/* unknown or non-select cte */)";
				}
				else {
                    out << "cte" << i << " AS (" << generateSelectQuery(*subSel, true) << ")";
				}

				if (i + 1 < with_queries.size()) {
					out << ", ";
				}
			}
			out << " ";
		}

		auto fields = query.selectable_fields();
		if (!fields.empty()) {
			out << "SELECT ";
			for (size_t i = 0; i < fields.size(); ++i) {
				out << fields[i]->get_name();
				if (i + 1 < fields.size()) out << ", ";
			}
		} else {
			out << "SELECT *";
		}
		out << " ";
		auto from_tables = query.from_tables();
		auto from_subqueries = query.from_subqueries();

		if (!from_tables.empty() || !from_subqueries.empty()) {
			out << "FROM ";
			bool needComma = false;

			for (const auto& i : from_tables) {
				if (needComma) {
					out << ", ";
				}
				out << i->name();
				needComma = true;
			}

			for (size_t i = 0; i < from_subqueries.size(); ++i) {
				if (needComma) {
					out << ", ";
				}
				auto subSel = std::dynamic_pointer_cast<select>(from_subqueries[i]);
				if (subSel) {
					out << "(" << generateSelectQuery(*subSel) << ") AS sub" << i;
				} else {
					out << "(/* unknown or non-select subquery */) AS sub" << i;
				}
				needComma = true;
			}
			out << " ";
		}

		auto joins = query.joins();
		for (size_t j = 0; j < joins.size(); ++j) {
			const auto& info = joins[j];

			out << select::to_string(info.join_type)
				<< " " << info.joined_rs->name() << " ";

			if (!info.on_conditions.empty()) {
				out << "ON ";

			bool multipleConds = (info.on_conditions.size() > 1);
			if (multipleConds) {
				out << "(";
			}

			for (size_t c = 0; c < info.on_conditions.size(); ++c) {
				const auto& cond = info.on_conditions[c];
				out << cond.left_field->get_name() << " "
					<< select::to_string(cond.op) << " ";
				printValue(out, cond.right_value);

				if (c + 1 < info.on_conditions.size()) {
					out << " AND ";
				}
			}

			if (multipleConds) {
				out << ")";
			}

			out << " ";
			}
		}

		auto where_conditions = query.where_conditions();
		if (!where_conditions.empty()) {
			out << "WHERE ";

			for (size_t i = 0; i < where_conditions.size(); ++i) {
				const auto& cond = where_conditions[i];
				out << cond.left_field->get_name() << " "
					<< select::to_string(cond.op) << " ";
				printValue(out, cond.right_value);


				if (i + 1 < where_conditions.size()) {
					out << " AND ";
				}
			}
			out << " ";
		}

		auto group_by_conditions = query.groupBy();

		if (!group_by_conditions.empty()) {
			out << "GROUP BY ";
			bool first = true;
			for (auto& fld : group_by_conditions) {
				if (!first) {
					out << ", ";
				}
				first = false;
				out << fld->get_name();
			}
			out << " ";
		}

		auto order_by = query.orderBy();
		if (!order_by.empty()) {
			out << "ORDER BY " << order_by << " ";
		}
        if(!isSubquery) {
            out << ";";
        }

		return out.str();
	}

	std::string	postgresql::generateCreateQuery(const create& query) const
	{
		std::ostringstream out;

		out << "CREATE TABLE ";
		if(query.use_if_not_exists()) {
			out << "IF NOT EXISTS ";
		}

		out << query.table_name() << " (";

		const auto cols = query.columns_list();
		bool needComma = false;
		for(const auto& column: cols) {
			if(needComma) {
				out << ", ";
			}

			out << column->to_SQL();
			needComma = true;
		}

		const auto pKeys = query.primary_keys();
		if (!pKeys.empty()) {
			out << ", PRIMARY KEY (";
			for (const auto& key: pKeys) {
				out << key;
				if (key != pKeys.back()) {
					out << ", ";
				}
			}
			out << ")";
		}

		const auto fKeys = query.foreign_keys();
		for (const auto& fk : fKeys) {
			out << ", ";

			if (fk.constraint_name.empty()) {
				throw std::logic_error("FOREIGN_KEY_NAME CANNOT be empty if key EXISTS!");
			}
			out << "CONSTRAINT " << fk.constraint_name << " ";

			out << "FOREIGN KEY (";
			for (const auto& fkCols: fk.columns) {
				out << fkCols;
				if (fkCols != fk.columns.back()) {
					out << ", ";
				}
			}

			out << ") REFERENCES " << fk.ref_table << "(";
			for (const auto& fkRef: fk.ref_columns) {
				out << fkRef;
				if (fkRef != fk.ref_columns.back()) {
					out << ", ";
				}
			}

			out << ")";

			if (fk.on_update.empty()) {
				out << ";";
				return out.str();
			}
			else out << " ON UPDATE " << fk.on_update;

			if (fk.on_delete.empty()) {
				out << ");";
				return out.str();
			}
			else out << " ON DELETE " << fk.on_delete;
		}

		out << ");";
		return out.str();
	}

	std::string	postgresql::generateDeleteQuery(const drop& query) const
	{
		std::ostringstream out;
		out << "DROP TABLE ";
		if (query.use_if_exists()) {
			out << "IF EXISTS ";
		}

		if(query.table_name().empty())
			throw std::logic_error("table to drop name CANNOT be empty at DROP TABLE query!");

		out << query.table_name() << ";";
		return out.str();
	}

	std::string	postgresql::generateInsertQuery(const insert& query) const
	{
		std::ostringstream out;
		out << "INSERT INTO " << query.table_name();

		const auto fields = query.columns_list();
		if(fields.empty()) {
			throw std::logic_error("INSERTABLE FIELDS CANNOT BE EMPTY!");
		}

		out << " (";
		for(const auto& field: fields) {
			out << field;
			if (field != fields.back()) {
				out << ", ";
			}
		}
		out << ")";

		const auto rows = query.rows();
		if(rows.empty()) {
			out << "DEFAULT VALUES";
			return out.str();
		}

		out << " VALUES ";
		for(const auto& row: rows) {
			out << "(";
			for(const auto& item: row) {
				printValue(out, item);
				if(item != row.back()) {
					out << ", ";
				}
			}
			out << ")";
		}

		out << ");";

		return out.str();
	}
}}

























