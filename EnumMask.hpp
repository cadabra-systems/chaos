//
//  EnumMask.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 03.04.17.
//  Copyright © 2017 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_EnumMask_hpp
#define Chaos_EnumMask_hpp

#include <set>
#include <numeric>
#include <type_traits>
#include <initializer_list>

namespace chaos {
	template<typename E>
	class enum_mask
	{
	/** @name Constructors */
	/** @{ */
	public:
		using container = std::set<E>;
		using const_iterator = typename container::const_iterator;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		enum_mask(bool all = false)
		{
			/// @todo
/*
			if (all) {
				_set.set();
			}
*/
		}

		enum_mask(std::initializer_list<E> e_list)
		:
			_set(e_list)
		{

		}

		enum_mask(container e_set)
		:
			_set(e_set)
		{

		}

		enum_mask(E flag)
		{
			_set.emplace(flag);
		}

		~enum_mask() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		container _set;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		typename std::enable_if<std::is_integral<typename std::underlying_type<E>::type>::value, typename std::underlying_type<E>::type>::type
		accumulate() const
		{
			return std::accumulate
			(
				_set.cbegin(), _set.cend(),
				static_cast<typename std::underlying_type<E>::type>(0),
				[](typename std::underlying_type<E>::type a, E b)
				{
					return a | static_cast<typename std::underlying_type<E>::type>(b);
				}
			);
		}

		bool test(E flag) const
		{
			return _set.find(flag) != _set.cend();
		}

		bool any(std::initializer_list<E> flag_list) const
		{
			const container& set(_set);
			return std::any_of(flag_list.begin(), flag_list.end(), [&set](E flag) { return set.find(flag) != set.cend(); } );
		}

		bool all(std::initializer_list<E> flag_list) const
		{
			const container& set(_set);
			return std::all_of(flag_list.begin(), flag_list.end(), [&set](E flag) { return set.find(flag) != set.cend(); } );
		}

		bool none(std::initializer_list<E> flag_list) const
		{
			const container& set(_set);
			return std::none_of(flag_list.begin(), flag_list.end(), [&set](E flag) { return set.find(flag) != set.cend(); } );
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		const container& get() const
		{
			return _set;
		}

		typename enum_mask<E>::const_iterator cbegin() const
		{
			return _set.cbegin();
		}

		typename enum_mask<E>::const_iterator cend() const
		{
			return _set.cend();
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set(E flag)
		{
			_set.emplace(flag);
		}

		void reset(E flag)
		{
			_set.erase(flag);
		}
	/** @} */
	};
}

#endif
