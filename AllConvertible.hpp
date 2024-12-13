//
//  AllConvertible.hpp
//  Cadabra
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_AllConvertible_hpp
#define Chaos_AllConvertible_hpp

#include <type_traits>

namespace chaos {
	template <typename T, typename... Ts>
	struct all_convertible : public std::conditional<(std::is_convertible_v<Ts&, T&> && ...), std::true_type, std::false_type>::type
	{
	};

	template <typename T, typename... Ts>
	constexpr bool all_convertible_v = all_convertible<T, Ts...>::value;
}

#endif
