//
//  BiMap.hpp
//  Chaos
//
//  Created by Daniil A Megrabyan on 05.11.17.
//  Copyright © 2017 Daniil A Megrabyan. All rights reserved.
//

#ifndef Chaos_BiMap3_hpp
#define Chaos_BiMap3_hpp

#include <map>
#include <algorithm>
#include <utility>
#include <memory>
#include <stdexcept>

namespace chaos {
	template <typename T1, typename T2>
	class bimap
	{
	/** @name Classes */
	/** @{ */
	private:
		template <typename T>
		struct internal_bimap_base
		{
			struct reverse
			{
			public:
				reverse()
				{
					
				}
				
				reverse(typename T::iterator p)
				:
					v(p)
				{
					
				}
				
			public:
				typename T::iterator v;
			};
			
			using reverse_map = std::map<T1, reverse>;
		};

		struct internal_bimap: internal_bimap_base<internal_bimap>
		{
			struct direct
			{
			public:
				direct()
				{
					
				}
				
				direct(typename internal_bimap_base<internal_bimap>::reverse_map::iterator p)
				:
					v(p)
				{
					
				}

			public:
				typename internal_bimap_base<internal_bimap>::reverse_map::iterator v;
			};
			
			using direct_map = std::map<T2, direct>;
			using iterator = typename direct_map::iterator;
		};
	/** @} */
		
	/** @name Aliases */
	/** @{ */
	public:
		using direct_map = typename internal_bimap::direct_map;
		using reverse_map = typename internal_bimap::reverse_map;
		using pair = std::pair<const T1&, const T2&>;
	/** @} */
		
	/** @name Classes */
	/** @{ */
	public:
		class const_iterator
		{
		friend class bimap;
			
		/** @name Aliases */
		/** @{ */	
		public:
			using iterator_category = std::input_iterator_tag;
			using value_type        = pair;
			using difference_type   = std::ptrdiff_t;
			using pointer           = const pair*;
			using reference         = const pair&;
		/** @} */

		/** @name Constructors */
		/** @{ */
		private:
			explicit const_iterator(const bimap& map, const typename direct_map::key_type& key)
			:
				_map(map),
				_i(_map._map1.find(key)),
				_pair(_map._map1.cend() != _i ? new pair(_i->second.v->first, _i->first) : nullptr)
			{

			}

			explicit const_iterator(const bimap& map, const typename reverse_map::key_type& key)
			:
				_map(map),
				_i(_map._map1.cend())
			{
				typename reverse_map::const_iterator i(_map._map2.find(key));
				if (_map._map2.cend() != i) {
					_i = i->second.v;
					_pair.reset(_map._map1.cend() != _i ? new pair(_i->second.v->first, _i->first) : nullptr);
				}
			}

			explicit const_iterator(const bimap& map, bool valid)
			:
				_map(map),
				_i(valid ? _map._map1.cbegin() : _map._map1.cend()),
				_pair(valid ? new pair(_i->second.v->first, _i->first) : nullptr)
			{

			}
		/** @} */
			
		/** @name Constructors */
		/** @{ */
		public:
			explicit const_iterator(const const_iterator& origin)
			:
				_map(origin._map),
				_i(origin._i),
				_pair(origin._pair ? new pair(origin._pair->first, origin._pair->second) : nullptr)
			{

			}

			explicit const_iterator(const_iterator&& origin)
			:
				_map(origin._map),
				_i(std::move(origin._i)),
				_pair(std::move(origin._pair))
			{

			}
		/** @} */
			
		/** @name Operators */
		/** @{ */
		private:
			const_iterator& operator=(const_iterator const& rhs)
			{
				if (&_map == &(rhs._map)) {
					_i = rhs._i;
					_pair.reset(_map._map1.cend() != _i ? new pair(rhs._i->second.v->first, rhs._i->first) : nullptr);
				}
				return *this;
			}
			
		public:
			bool operator!=(const_iterator const& rhs) const
			{
				return _i != rhs._i;
			}
			
			bool operator==(const_iterator const& rhs) const
			{
				return _i == rhs._i;
			}
			
			const std::pair<const T1&, const T2&>& operator*() const
			{
				return *_pair;
			}
			
			const std::pair<const T1&, const T2&>* operator->() const
			{
				return _pair.get();
			}
			
			const_iterator& operator++()
			{
				++_i;
				_pair.reset(new pair(_i->second.v->first, _i->first));
				return *this;
			}
			
			const_iterator operator++(int)
			{
				++_i;
				_pair.reset(new pair(_i->second.v->first, _i->first));
				return *this;
			}
			
			const_iterator& operator--()
			{
				--_i;
				_pair.reset(new pair(_i->second.v->first, _i->first));
				return *this;
			}
			
			const_iterator& operator--(int)
			{
				--_i;
				_pair.reset(new pair(_i->second.v->first, _i->first));
				return *this;
			}
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			const bimap& _map;
			typename direct_map::const_iterator _i;
			std::unique_ptr<pair> _pair;
		/** @} */
		};
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		direct_map _map1;
		reverse_map _map2;

		const_iterator _default;
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		bimap()
		:
			_default(*this, false)
		{
			
		}
		
		bimap(const std::initializer_list<std::pair<T1, T2>>& list)
		:
			_default(*this, false)
		{
			for(const std::pair<T1, T2>& value : list) {
				emplace(value);
			}
		}

		bimap(std::initializer_list<std::pair<T1, T2>>&& list)
		:
			_default(*this, false)
		{
			for(const std::pair<T1, T2>& value : list) {
				emplace(value);
			}
		}

		bimap(const std::initializer_list<std::pair<T1, T2>> list, T1&& key)
		:
			bimap(list, key)
		{

		}

		bimap(const std::initializer_list<std::pair<T1, T2>> list, T2&& key)
		:
			bimap(list, key)
		{

		}
		
		bimap(const std::initializer_list<std::pair<T1, T2>> list, const T1& key)
		:
			_default(*this, false)
		{
			for(const std::pair<T1, T2>& value : list) {
				emplace(value);
			}
			_default = const_iterator(*this, key);
		}
		
		bimap(const std::initializer_list<std::pair<T1, T2>> list, const T2& key)
		:
			_default(*this, false)
		{
			for(const std::pair<T1, T2>& value : list) {
				emplace(value);
			}
			_default = const_iterator(*this, key);
		}

		bimap(bimap&& origin)
		:
			_map1(std::move(origin._map1)),
			_map2(std::move(origin._map2)),
			_default(std::move(origin._default))
		{
			
		}
		
		bimap(const bimap& origin)
		:
			_map1(origin._map1),
			_map2(origin._map2),
			_default(origin._default)
		{
			
		}
		
		~bimap() = default;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		bool emplace(const std::pair<T1, T2>& pair)
		{
			std::pair<typename direct_map::iterator, bool> emplace1(_map1.emplace(pair.second, _map2.end()));
			if (!emplace1.second) {
				return false;
			}
			
			std::pair<typename reverse_map::iterator, bool> emplace2(_map2.emplace(pair.first, emplace1.first));
			if (!emplace2.second) {
				_map1.erase(emplace1.first);
				return false;
			}
			
			emplace1.first->second = emplace2.first;
			return true;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const T2& at(const T1& key, bool use_default = false) const
		{
			typename reverse_map::const_iterator i(_map2.find(key));
			if (_map2.cend() != i) {
				return i->second.v->first;
			} else if (use_default && cend() == _default) {
				throw std::out_of_range("bimap::at key not found");
			}
			return _default->second;
		}
		
		const T1& at(const T2& key, bool use_default = false) const
		{
			typename direct_map::const_iterator i(_map1.find(key));
			if (_map1.cend() != i) {
				return i->second.v->first;
			} else if (use_default && cend() == _default) {
				throw std::out_of_range("bimap::at key not found");
			}
			return _default->first;
		}

		typename direct_map::size_type size() const noexcept
		{
			return _map1.size();
		}
		
		typename reverse_map::size_type count(const T1& key) const noexcept
		{
			return _map2.count(key);
		}
		
		typename direct_map::size_type count(const T2& key) const noexcept
		{
			return _map1.count(key);
		}
		
		const_iterator find(const T1& key) const noexcept
		{
			return const_iterator(*this, key);
		}
		
		const_iterator find(const T2& key) const noexcept
		{
			return const_iterator(*this, key);
		}
		
		const_iterator cbegin() const noexcept
		{
			return const_iterator(*this, true);
		}
		
		const_iterator cend() const noexcept
		{
			return const_iterator(*this, false);
		}
/*
		const_reverse_iterator crbegin() const noexcept
		{
			return _set.crbegin();
		};
		
		const_reverse_iterator crend() const noexcept
		{
			return _set.crend();
		};
*/
	/** @} */
		
	/** @name States  */
	/** @{ */
	public:
		bool empty() const noexcept
		{
			return _map1.empty();
		}
		
		bool has(const T1& key) const noexcept
		{
			return count(key) > 0;
		}
		
		bool has(const T2& key) const noexcept
		{
			return count(key) > 0;
		}
	/** @} */
	};
}

#endif
