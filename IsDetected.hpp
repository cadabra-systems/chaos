/**
 @file IsDetected.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_IsDetected_hpp
#define Chaos_IsDetected_hpp

#include "VoidType.hpp"

#include <type_traits>

namespace chaos {
	template <template <class...> class Trait, class Enabler, class... Args>
	struct __is_detected : std::false_type{};

	template <template <class...> class Trait, class... Args>
	struct __is_detected<Trait, void_t<Trait<Args...>>, Args...> : std::true_type{};

	template <template <class...> class Trait, class... Args>
	using is_detected = typename __is_detected<Trait, void, Args...>::type;
}

#endif
