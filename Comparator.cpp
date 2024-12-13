/**
 @file Comparator.cpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Comparator.hpp"

namespace chaos {
	const bimap<std::string, comparator::op> comparator::map = {
		{
			{">", comparator::op::greater},
			{">=", comparator::op::greater_or_equal},
			{"=", comparator::op::equal},
			{"<", comparator::op::less},
			{"<=", comparator::op::less_or_equal},
			{"∈", comparator::op::contains}
		},
		comparator::op::equal
	};

	comparator::comparator(comparator::op operator_type_)
	:
		operator_type(operator_type_)
	{

	}
}
