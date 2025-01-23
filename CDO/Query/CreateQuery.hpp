/**
 @file CreateQuery.hpp
 @date 05.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_Create_Query_hpp
#define Chaos_CDO_Create_Query_hpp

#include "../AbstractQuery.hpp"
#include "../AbstractField.hpp"
#include "../Table.hpp"

#include <string>
#include <memory>
#include <vector>


namespace chaos { namespace cdo {
	class create: public abstract_query
	{
	/** @name Classes */
	/** @{ */
	public:
		struct foreign_key {
			std::string constraint_name;
			std::vector<std::string> columns;
			std::string ref_table;
			std::vector<std::string> ref_columns;
			std::string on_update;
			std::string on_delete;
		};
	/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		create(const std::string& name, bool ifNotExists = true); // if no table was created before
		create(const table& tbl, bool ifNotExists = true); // if we want to use existing table

		create(const create&) = default;
		create(create&&) = default;

		create& operator=(const create&) = default;
		create& operator=(create&&) = default;

		virtual ~create() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
	/** @} */

		std::string _table_name;
		bool _if_not_exists;
		std::vector<std::shared_ptr<abstract_field>> _columns;
		std::vector<std::string> _primary_keys;
		std::vector<struct foreign_key> _foreign_keys;


	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */
	/** @name Setters */
	/** @{ */
	public:
		create& if_not_exists(bool state = true);

		create& columns(std::shared_ptr<abstract_field> field);
		create& columns(std::initializer_list<std::shared_ptr<abstract_field>> fields);
		create& columns(const std::vector<std::shared_ptr<abstract_field>>& fields);

		create& primary_key(const std::vector<std::string>& keys);
		create& primary_key(std::initializer_list<std::string> keys);

		create& foreign_key(const std::vector<foreign_key>& keys);
		create& foreign_key(std::initializer_list<struct foreign_key> keys);
	/** @} */
	/** @name Getters */
	/** @{ */
	public:
		std::string table_name() const {return _table_name;};
		bool use_if_not_exists() const {return _if_not_exists;};
		std::vector<std::shared_ptr<abstract_field>> columns_list() const {return _columns;};
		std::vector<std::string> primary_keys() const {return _primary_keys;};
		std::vector<struct foreign_key> foreign_keys() const {return _foreign_keys;};
	/** @} */
	};
}}
#endif // CREATEQUERY_HPP
