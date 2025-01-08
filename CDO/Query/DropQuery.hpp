/**
 @file DropQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_Drop_Query_hpp
#define Chaos_CDO_Drop_Query_hpp

#include "../AbstractQuery.hpp"
#include "../Table.hpp"

#include <string>


namespace chaos { namespace cdo {
	class drop : public abstract_query {

	/** @name Classes */
	/** @{ */
	public:
	/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		drop(const std::string& tableName, bool ifExists = false);
		drop(const table& table, bool ifExists = false);

		drop(const drop&) = default;
		drop(drop&&) = default;

		drop& operator=(const drop&) = default;
		drop& operator=(drop&&) = default;

		bool operator==(const drop& other) const;
		virtual ~drop() = default;

	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _table_name;
		bool _if_exists;
	/** @} */
	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */
	/** @name Setters */
	/** @{ */
	public:
		drop& if_exists(bool ifExists = true);
	/** @} */
	/** @name Getters */
	/** @{ */
	public:
		std::string table_name() const {return _table_name;};
		bool use_if_exists() const {return _if_exists;};
	/** @} */
	};
}}


#endif // drop_HPP
