//
//  BitMasquerade.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 03.04.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_BitMasquerade_hpp
#define Chaos_BitMasquerade_hpp

#include <type_traits>

namespace chaos {
	/// @todo SFINAE to exlclude all enum
	template<typename E>
	struct enum_bitmasquerade
	{
	private:
		using type = E;

		static void detect(...);

		template<typename U>
		static decltype(std::declval<U>().foo(42)) detect(const U&);

	public:
		static constexpr bool value = std::is_same<void, decltype(detect(std::declval<E>()))>::value;
	};

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator~(E lhs)
	{
		return static_cast<E>(~static_cast<typename std::underlying_type<E>::type>(lhs));
	}

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator|(E lhs, E rhs)
	{
		return static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) | static_cast<typename std::underlying_type<E>::type>(rhs));
	}

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator&(E lhs, E rhs)
	{
		return static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) & static_cast<typename std::underlying_type<E>::type>(rhs));
	}

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator^(E lhs, E rhs)
	{
		return static_cast<E>(static_cast<typename std::underlying_type<E>::type>(lhs) ^ static_cast<typename std::underlying_type<E>::type>(rhs));
	}

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator|=(E& lhs, E rhs)
	{
		return reinterpret_cast<E&>(reinterpret_cast<typename std::underlying_type<E>::type&>(lhs) |= static_cast<typename std::underlying_type<E>::type>(rhs));
	}

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator&=(E& lhs, E rhs)
	{
		return reinterpret_cast<E&>(reinterpret_cast<typename std::underlying_type<E>::type&>(lhs) &= static_cast<typename std::underlying_type<E>::type>(rhs));
	}

	template<typename E>
	constexpr inline typename std::enable_if<enum_bitmasquerade<E>::enable, E>::type operator^=(E& lhs, E rhs)
	{
		return reinterpret_cast<E&>(reinterpret_cast<typename std::underlying_type<E>::type&>(lhs) ^= static_cast<typename std::underlying_type<E>::type>(rhs));
	}
}

#endif
