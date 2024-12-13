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
#include <initializer_list>

namespace chaos {
	template<typename E>
	class enum_mask
	{
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

		enum_mask(std::set<E> e_set)
		:
			_set(e_set)
		{

		}

		enum_mask(E flag)
		{
			_set.emplace(flag);
		}

		~enum_mask() = default;

	public:
		void set(E flag)
		{
			_set.emplace(flag);
		}

		void reset(E flag)
		{
			_set.erase(flag);
		}

		bool test(E flag) const
		{
			return _set.find(flag) != _set.cend();
		}

		bool any(std::initializer_list<E> flag_list) const
		{
			const std::set<E>& set(_set);
			return std::any_of(flag_list.begin(), flag_list.end(), [&set](E flag) { return set.find(flag) != set.cend(); } );
		}

		bool all(std::initializer_list<E> flag_list) const
		{
			const std::set<E>& set(_set);
			return std::all_of(flag_list.begin(), flag_list.end(), [&set](E flag) { return set.find(flag) != set.cend(); } );
		}

		bool none(std::initializer_list<E> flag_list) const
		{
			const std::set<E>& set(_set);
			return std::none_of(flag_list.begin(), flag_list.end(), [&set](E flag) { return set.find(flag) != set.cend(); } );
		}
	private:
		std::set<E> _set;
	};
}

#endif
