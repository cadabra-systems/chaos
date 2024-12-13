/**
 @file Comparator.hpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Comparator_hpp
#define Chaos_Comparator_hpp

#include "BiMap.hpp"

#include <string>
#include <algorithm>

namespace chaos {
	class comparator
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class op : char
		{
			less = 'l',
			less_or_equal = 'L',
			equal = '=',
			greater = 'g',
			greater_or_equal = 'G',
			contains = '^'
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const bimap<std::string, comparator::op> map;

		template <typename T>
		static bool is(const T& lhs, comparator::op operator_type, const T& rhs)
		{
			switch (operator_type) {
				case comparator::op::equal: return lhs == rhs;
				case comparator::op::less: return lhs < rhs;
				case comparator::op::less_or_equal: return lhs <= rhs;
				case comparator::op::greater: return lhs > rhs;
				case comparator::op::greater_or_equal: return lhs >= rhs;
				case comparator::op::contains: return is_contain(rhs, lhs);
			}
			return false;
		}

		template <typename T>
		static bool is_contain(const T& lhs, const T& rhs)
		{
			const T interesction(lhs & rhs);
			return interesction == lhs || interesction == rhs;
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		comparator(comparator::op op);
		~comparator() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	public:
		comparator::op operator_type;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		template <typename T>
		bool operator()(const T& lhs, const T& rhs) const
		{
			return comparator::is(lhs, operator_type, rhs);
		}
	/** @} */
	};
}

#endif
