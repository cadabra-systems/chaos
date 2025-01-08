/**
 @file DeleteQuery.hpp
 @date 06.01.2025
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_Delete_Query_hpp
#define Chaos_CDO_Delete_Query_hpp

#include "../AbstractQuery.hpp"
#include "../Table.hpp"

#include <string>


namespace chaos { namespace cdo {
	class delete_query : public abstract_query {

	/** @name Classes */
	/** @{ */
	public:
	/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		delete_query(const std::string& tableName, bool ifExists = false);
		delete_query(const table& table, bool ifExists = false);

		delete_query(const delete_query&) = default;
		delete_query(delete_query&&) = default;

		delete_query& operator=(const delete_query&) = default;
		delete_query& operator=(delete_query&&) = default;

		bool operator==(const delete_query& other) const;
		virtual ~delete_query() = default;

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
		delete_query& if_exists(bool ifExists = true);
		delete_query& with(const abstract_query& query);

		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);
		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);
		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);

		delete_query& returning(std::shared_ptr<abstract_field> field);
		delete_query& returning(const std::vector<std::shared_ptr<abstract_field>>& fields);
		delete_query& returning(std::initializer_list<std::shared_ptr<abstract_field>> fields);

		delete_query& returning(const std::string& field);
		delete_query& returning(const std::vector<std::string>& fields);
		delete_query& returning(std::initializer_list<std::string> fields);
	/** @} */
	/** @name Getters */
	/** @{ */
	public:
		std::string table_name() const {return _table_name;};
		bool use_if_exists() const {return _if_exists;};
	/** @} */
	};
}}


#endif // delete_query_HPP
