//
//  Function.hpp
//  Abra
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Function_hpp
#define Chaos_Function_hpp

#include <type_traits>
#include <cstdio>

namespace chaos {
	class function
	{
	/** @name Classes */
	/** @{ */
	public:
		template <std::size_t N, typename T0, typename ... Ts>
		struct n_type
		{
			using value = typename n_type<N - 1U, Ts...>::value;
		};

		template <typename T0, typename ... Ts>
		struct n_type<0U, T0, Ts...>
		{
			using value = T0;
		};

		template <std::size_t, typename>
		struct argument;

		template <std::size_t N, typename R, typename ... As>
		struct  argument<N, R(As...)>
		{
			using type = typename n_type<N, As...>::value;
		};

		template <typename>
		struct return_type;

		template <typename R, typename ... As>
		struct return_type<R(As...)>
		{
			using value = R;
		};
	/** @} */
	};
}

#endif
