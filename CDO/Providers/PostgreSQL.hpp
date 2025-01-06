/**
 @file PosgtreSQL.hpp
 @date 28.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_PosgtreSQL_hpp
#define Chaos_CDO_PosgtreSQL_hpp

#include "../SQL.hpp"
#include "../Query/CreateQuery.hpp"
#include "../Query/DeleteQuery.hpp"
#include "../Query/InsertQuery.hpp"
#include "../Query/SelectQuery.hpp"



namespace chaos { namespace cdo {
	class postgresql : public sql
	{
	/** @name Constructors */
	/** @{ */
	public:
		postgresql() = default;
		postgresql(const postgresql&) = delete;
		postgresql& operator=(const postgresql&) = delete;
		virtual ~postgresql() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
		virtual std::string	generateSelectQuery(const select& query) const override;
		virtual std::string	generateCreateQuery(const create& query) const override;
		virtual std::string	generateDeleteQuery(const drop& query) const override;
		virtual std::string	generateInsertQuery(const insert& query) const override;
		virtual std::string	generateUpdateQuery(const abstract_query& query) const override {return "";};


	private:
		std::string escape_string(const std::string& input) const;
		void printValue(std::ostream& out, const std::variant<std::shared_ptr<abstract_field>, int, std::string>& v) const;
		/** @} */
	/** @name Getters */
	/** @{ */
	public:
	/** @} */
	};
}}
#endif // POSTGRESQL_HPP
