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
#include "../View.hpp"

#include <string>


namespace chaos { namespace cdo {
	class delete_query : public abstract_query {
	/** @name Aliases */
	/** @{ */
	public:
		using UsingItem = std::variant<std::shared_ptr<abstract_query>, std::shared_ptr<row_set>>;
	/** @} */
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
		virtual ~delete_query() override = default;

	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _table_name;
		bool _if_exists;
		std::vector<UsingItem> _using;

	/** @} */
	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */
	/** @name Setters */
	/** @{ */
	public:
		delete_query& as(const std::string& name);
		delete_query& asAlias(const std::string& alias);

		delete_query& if_exists(bool ifExists = true);

		delete_query& with(const abstract_query& anchor, const std::string& alias = "");
		delete_query& with(const abstract_query& anchor, const abstract_query& reqursive, const std::string& alias = "", QueryUnionType type = QueryUnionType::UnionAll);

		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal);
		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		delete_query& where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		delete_query& where(const abstract_query& left, ECompareOp op, const abstract_query& rightVal);
		delete_query& where(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& where(const abstract_query& left, ECompareOp op, int rightVal);
		delete_query& where(const abstract_query& left, ECompareOp op, const std::string& rightVal);

		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal);
		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		delete_query& and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		delete_query& and_(const abstract_query& left, ECompareOp op, const abstract_query& rightVal);
		delete_query& and_(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& and_(const abstract_query& left, ECompareOp op, int rightVal);
		delete_query& and_(const abstract_query& left, ECompareOp op, const std::string& rightVal);

		delete_query& or_(std::shared_ptr<abstract_field> left, ECompareOp op, const abstract_query& rightVal);
		delete_query& or_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& or_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		delete_query& or_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		delete_query& or_(const abstract_query& left, ECompareOp op, const abstract_query& rightVal);
		delete_query& or_(const abstract_query& left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		delete_query& or_(const abstract_query& left, ECompareOp op, int rightVal);
		delete_query& or_(const abstract_query& left, ECompareOp op, const std::string& rightVal);

		delete_query& using_(std::shared_ptr<abstract_query> query);
		delete_query& using_(const abstract_query& query);
		delete_query& using_(std::shared_ptr<table> query);
		delete_query& using_(const table& query);
		delete_query& using_(std::shared_ptr<view> query);
		delete_query& using_(const view& query);

		delete_query& using_(const std::vector<std::shared_ptr<abstract_query>>& queries);
		delete_query& using_(const std::vector<abstract_query>& queries);
		delete_query& using_(const std::vector<std::shared_ptr<table>>& queries);
		delete_query& using_(const std::vector<table>& queries);
		delete_query& using_(const std::vector<std::shared_ptr<view>>& queries);
		delete_query& using_(const std::vector<view>& queries);

		delete_query& using_(std::initializer_list<std::shared_ptr<abstract_query>> queries);
		delete_query& using_(std::initializer_list<abstract_query> queries);
		delete_query& using_(std::initializer_list<std::shared_ptr<table>> queries);
		delete_query& using_(std::initializer_list<table> queries);
		delete_query& using_(std::initializer_list<std::shared_ptr<view>> queries);
		delete_query& using_(std::initializer_list<view> queries);

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
		std::vector<UsingItem> get_using() const {return _using;};
	/** @} */
	};
}}


#endif // delete_query_HPP
