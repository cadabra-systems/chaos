/**
 @file InsertQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_Insert_Query_hpp
#define Chaos_CDO_Insert_Query_hpp

#include "../AbstractQuery.hpp"
#include "../Table.hpp"

#include <initializer_list>
#include <string>
#include <variant>
#include <vector>


namespace chaos { namespace cdo {
	class insert : public abstract_query {

	/** @name Alias*/
	public:
		using InsertValue = std::variant<std::shared_ptr<abstract_field>, std::shared_ptr<row_set>, std::shared_ptr<abstract_query>, int, std::string>;
		using RowType = std::vector<InsertValue>;
	/** @{ */
	public:
	/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		insert(const std::string& tableName);
		insert(const table& table, bool useAllFields = false);

		insert(const insert&) = default;
		insert(insert&&) = default;

		insert& operator=(const insert&) = default;
		insert& operator=(insert&&) = default;

		bool operator==(const insert& other) const;

		virtual ~insert() override = default;

	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _table_name;
		bool _use_all_fields;

		std::shared_ptr<select> _select = nullptr;
		std::vector<std::shared_ptr<abstract_field>> _insert_into; // where to insert
		std::vector<RowType> _rows; // stuff to insert

	/** @} */
	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */
	/** @name Setters */
	/** @{ */
	public:
		insert& with(const abstract_query& anchor, const std::string& alias = "");
		insert& with(const abstract_query& anchor, const abstract_query& reqursive, const std::string& alias = "", QueryUnionType type = QueryUnionType::UnionAll);

		insert& columns(std::shared_ptr<abstract_field> cols);
		insert& columns(const std::vector<std::shared_ptr<abstract_field>> &cols);
		insert& columns(std::initializer_list<std::shared_ptr<abstract_field>> cols);

		insert& values(const RowType& row);
		insert& values(std::initializer_list<InsertValue> row);
		insert& values(std::initializer_list<RowType> rows);
		insert& values(const std::vector<RowType>& rows);

		insert& insert_from_select(std::shared_ptr<select> query);

		insert& returning(std::shared_ptr<abstract_field> field);
		insert& returning(const std::vector<std::shared_ptr<abstract_field>>& fields);
		insert& returning(std::initializer_list<std::shared_ptr<abstract_field>> fields);

		insert& returning(const std::string& field);
		insert& returning(const std::vector<std::string>& fields);
		insert& returning(std::initializer_list<std::string> fields);

	/** @} */
	/** @name Getters */
	/** @{ */
	public:
		inline std::string table_name() const {return _table_name;};
		inline std::vector<std::shared_ptr<abstract_field>> columns_list() const {return _insert_into;};
		inline std::vector<RowType> rows() const {return _rows;};
		inline std::shared_ptr<select> selectable() const {return _select;};
	/** @} */
	};
}}


#endif // insert_HPP
