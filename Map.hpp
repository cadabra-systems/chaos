/**
 @file Map.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Map_hpp
#define Chaos_Map_hpp

#include <string>
#include <algorithm>
#include <iostream>
#include <cassert>

namespace chaos {
	template <template<typename, typename, typename...> class C, typename K, typename V, typename... Args>
	inline V map_value_default(const C<K, V, Args...>& m, K const& key, const V& default_value)
	{
		typename C<K,V,Args...>::const_iterator i(m.find(key));
		return m.end() == i ? default_value : i->second;
	}
}
#endif
