#include "SQL.hpp"

namespace chaos { namespace cdo {

	std::string sql::processQuery(const abstract_query& query, bool isSubquery) const
	{
		if (auto sel = dynamic_cast<const select*>(&query)) { // or maybe any other way to check
			return generateSelectQuery(*sel, isSubquery);
		}

		if (auto sel = dynamic_cast<const create*>(&query)) { // create
			return generateCreateQuery(*sel);
		}

		if (auto sel = dynamic_cast<const delete_query*>(&query)) { // delete
			return generateDeleteQuery(*sel, isSubquery);
		}

		if (auto sel = dynamic_cast<const drop*>(&query)) { // drop
			return generateDropQuery(*sel, isSubquery);
		}

		if (auto sel = dynamic_cast<const insert*>(&query)) { // insert
			return generateInsertQuery(*sel, isSubquery);
		}

		// if (auto sel = dynamic_cast<const update*>(&query)) { // update
		// 	return generateUpdateQuery(*sel, isSubquery);
		// }

		throw std::runtime_error("Unsupported query type at sql::processQuery()");
	}



	std::string sql::operator()(const abstract_query& query) const
	{
		return processQuery(query);
	}

}}
