/**
 @file AbstractQuery.hpp
 @date 25.12.2024
 @copyright Cadabra Systems
 @author avlukyanov01@gmail.com
 */

#ifndef Chaos_CDO_Abstract_Query_hpp
#define Chaos_CDO_Abstract_Query_hpp

#include "AbstractField.hpp"

#include <memory>
#include <vector>
#include <string>

namespace chaos { namespace cdo {
	class abstract_query
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class ECompareOp {
			Equal = 0,
			NotEqual,
			Greater,
			Less,
			GreaterOrEqual,
			LessOrEqual,
		};

		inline static std::string to_string(const ECompareOp& op)
		{
			switch (op)
			{
			case ECompareOp::Equal:           return "=";
			case ECompareOp::NotEqual:        return "<>";
			case ECompareOp::Greater:         return ">";
			case ECompareOp::Less:            return "<";
			case ECompareOp::GreaterOrEqual:  return ">=";
			case ECompareOp::LessOrEqual:     return "<=";
			}
			return "";
		}


		/**
		* @brief Структура для хранения одного условия WHERE
		*/
		struct Condition {
			std::shared_ptr<abstract_field> left_field;
			ECompareOp op;
			std::variant<std::shared_ptr<abstract_field>, int, std::string> right_value;

			bool operator==(const Condition& rhs) const
			{
				return left_field == rhs.left_field &&
					   op == rhs.op &&
					   right_value == rhs.right_value;
			}
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		abstract_query() = default;

		virtual ~abstract_query() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::vector<std::shared_ptr<abstract_query>> _with_queries;
		std::vector<Condition> _where_conditions;
		std::vector<std::string> _returning;
	/** @} */

	/** @name Setters */
	/** @{ */
	protected:
		void add_where_condition(const Condition& condition) {_where_conditions.push_back(condition);} // add check on null ptrs
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		inline std::vector<std::shared_ptr<abstract_query>> with_queries() const {return _with_queries;};
		inline std::vector<Condition> where_conditions() const { return _where_conditions; }
		inline std::vector<std::string> returning_list() const {return _returning;};
	/** @} */
	};
}}
#endif
