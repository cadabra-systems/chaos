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
#include "Query/DropQuery.hpp"
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
		virtual ~sql() = 0;
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
		virtual std::string	generateDeleteQuery(const delete_query& query, bool isSubquery = false) const = 0;
		virtual std::string	generateDropQuery(const drop& query, bool isSubquery = false) const = 0;
		virtual std::string	generateInsertQuery(const insert& query, bool isSubquery = false) const = 0;
		virtual std::string	generateUpdateQuery(const abstract_query& query) const = 0;

		std::string processQuery(const abstract_query& query, bool isSubquery = false) const;
	/** @name Getters */
	/** @{ */
	public:
		std::string operator()(const abstract_query& query) const;
	/** @} */
	};
}}
#endif
