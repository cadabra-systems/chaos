/**
 @file ArrayType.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_ArrayType_hpp
#define Chaos_ArrayType_hpp

#include <type_traits>

namespace chaos {
	template <typename T, typename = void>
	struct array_type : std::false_type
	{
		using element_type = void;
	};

	template <typename T>
	struct array_type<T, std::void_t<decltype(std::declval<T&>().emplace_back(std::declval<typename T::value_type>()))>> : std::true_type
	{
		using element_type = typename T::value_type;
	};
}

#endif
