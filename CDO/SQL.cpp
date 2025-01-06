#include "SQL.hpp"

namespace chaos { namespace cdo {

	std::string sql::operator()(const abstract_query& query) const
	{
		if (auto sel = dynamic_cast<const select*>(&query)) { // or maybe any other way to check
			return generateSelectQuery(*sel);
		}

		if (auto sel = dynamic_cast<const create*>(&query)) { // create
			return generateCreateQuery(*sel);
		}

		if (auto sel = dynamic_cast<const drop*>(&query)) { // delete
			return generateDeleteQuery(*sel);
		}

		if (auto sel = dynamic_cast<const insert*>(&query)) { // insert
			return generateInsertQuery(*sel);
		}

		// if (auto sel = dynamic_cast<const update*>(&query)) { // update
		// 	return generateUpdateQuery(*sel);
		// }

		throw std::runtime_error("Unsupported query type");
	}

}}
