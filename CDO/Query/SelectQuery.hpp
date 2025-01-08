/**
 @file SelectQuery.hpp
 @date 25.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
*/

#ifndef Chaos_CDO_Select_Query_hpp
#define Chaos_CDO_Select_Query_hpp

#include "../AbstractQuery.hpp"
#include "../Table.hpp"
#include "../View.hpp"

#include <string>
#include <memory>
#include <variant>
#include <vector>

namespace chaos { namespace cdo {
	class select : public abstract_query
	{
	/** @name Classes */
	/** @{ */
	public:

		enum class EJoinType {
			Inner,			// Внутреннее соединение
			Left,			// Левое соединение
			Right,			// Правое соединение
			Full,			// Полное внешнее соединение
			Cross,			// Декартово произведение
			Natural,		// Естественное соединение
			LeftExclusion,  // Только строки из левой таблицы, которых нет в правой
			RightExclusion, // Только строки из правой таблицы, которых нет в левой
			FullExclusion,  // Только строки без пересечений (из обеих таблиц)
			SelfJoin        // Соединение таблицы с самой собой
		};

		inline static std::string to_string(const EJoinType& jt)
		{
			switch (jt)
			{
			case EJoinType::Inner:           return "INNER JOIN";
			case EJoinType::Left:            return "LEFT JOIN";
			case EJoinType::Right:           return "RIGHT JOIN";
			case EJoinType::Full:            return "FULL JOIN";
			case EJoinType::Cross:           return "CROSS JOIN";
			case EJoinType::Natural:         return "NATURAL JOIN";
			case EJoinType::LeftExclusion:   return "LEFT EXCLUSION JOIN";
			case EJoinType::RightExclusion:  return "RIGHT EXCLUSION JOIN";
			case EJoinType::FullExclusion:   return "FULL EXCLUSION JOIN";
			case EJoinType::SelfJoin:        return "SELF JOIN";
			}
			return "";

		}

		struct JoinInfo {
			std::shared_ptr<row_set> joined_rs;
			EJoinType join_type;
			std::vector<Condition> on_conditions;

			bool operator==(const JoinInfo& rhs) const
			{
				return joined_rs == rhs.joined_rs &&
					   join_type == rhs.join_type &&
					   on_conditions == rhs.on_conditions;
			}

		};

	/** @} */
	/** @name Constructors */
	/** @{ */
	public:
		select() = default;
		select(const select&) = default;
		select& operator=(const select&) = default;
		select(select&&) = default;
		select& operator=(select&&) = default;

		bool operator==(const select& other) const;

		virtual ~select() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		// SELECTABLE FIELDS
		std::vector<std::shared_ptr<abstract_field>> _selectable_fields;

		// FROM
		std::vector<std::shared_ptr<row_set>> _from_tables;
		std::vector<std::shared_ptr<abstract_query>> _from_subqueries;

		// JOIN
		std::vector<JoinInfo> _joins;

		// GROUP BY
		std::vector<std::shared_ptr<abstract_field>> _group_by_fields;

		// ORDER BY
		std::string _order_by;

	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		select& with(const abstract_query& cte);

		select& fields(std::shared_ptr<abstract_field> field);
		select& fields(const std::vector<std::shared_ptr<abstract_field>>& fields);
		select& fields(std::initializer_list<std::shared_ptr<abstract_field>> fields);

		select& from(const table& t);
		select& from(const view& v);
		select& from(const abstract_query& subq);

		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		select& where(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);

		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> rightVal);
		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, int rightVal);
		select& and_(std::shared_ptr<abstract_field> left, ECompareOp op, const std::string& rightVal);



		select& join_inner(const row_set& rs);
		select& join_left(const row_set& rs);
		select& join_right(const row_set& rs);
		select& join_full(const row_set& rs);
		select& join_cross(const row_set& rs);
		select& join_natural(const row_set& rs);
		select& join_left_exclusion(const row_set& rs);
		select& join_right_exclusion(const row_set& rs);
		select& join_full_exclusion(const row_set& rs);
		select& join_self(const row_set& rs);
		select& on(std::shared_ptr<abstract_field> left, ECompareOp op, std::shared_ptr<abstract_field> right);

		select& order(std::shared_ptr<abstract_field> field, bool ascending = true);
		select& group(std::shared_ptr<abstract_field> field);

	private:
		select& join(const row_set& rs, EJoinType type);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:

		std::vector<std::shared_ptr<abstract_field>> selectable_fields() const {return _selectable_fields;};
		std::vector<std::shared_ptr<row_set>> from_tables() const {return _from_tables;};
		std::vector<std::shared_ptr<abstract_query>> from_subqueries() const {return _from_subqueries;};
		std::vector<JoinInfo> joins() const {return _joins;};
		std::vector<Condition> where_conditions() const {return _where_conditions;};
		std::vector<std::shared_ptr<abstract_field>> groupBy() const {return _group_by_fields;};
		const std::string orderBy() const {return _order_by;};
	/** @} */
	};
}}

#endif
