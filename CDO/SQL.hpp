/**
 @file SQL.hpp
 @date 28.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_SQL_hpp
#define Chaos_CDO_SQL_hpp

#include "AbstractQuery.hpp"

#include "Query/CreateQuery.hpp"
#include "Query/DeleteQuery.hpp"
#include "Query/InsertQuery.hpp"
#include "Query/SelectQuery.hpp"

#include <string>
namespace chaos { namespace cdo {
	class sql
	{
	/** @name Constructors */
	/** @{ */
	public:
		sql() = default;
		virtual ~sql() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */
        virtual std::string	generateSelectQuery(const select& query, bool isSubquery = false) const = 0;
		virtual std::string	generateCreateQuery(const create& query) const = 0;
		virtual std::string	generateDeleteQuery(const drop& query) const = 0;
		virtual std::string	generateInsertQuery(const insert& query) const = 0;
		virtual std::string	generateUpdateQuery(const abstract_query& query) const = 0;
	/** @name Getters */
	/** @{ */
	public:
		std::string operator()(const abstract_query& query) const;
	/** @} */
	};
}}
#endif
