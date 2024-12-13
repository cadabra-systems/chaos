/**
 @file DefectBiMap.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_DefectBiMap_hpp
#define Chaos_DefectBiMap_hpp

#include <set>
#include <utility>

namespace chaos {	
	template <typename T1, typename T2>
	class bimap
	{
	/** @name Classes */
	/** @{ */
	private:
		struct comparator
		{
			using is_transparent = void;
			
			bool operator()(const std::pair<T1, T2>& pair1, const std::pair<T1, T2>& pair2) const
			{
				return pair1 < pair2;
			}
			
			bool operator()(const T1& argument1, const std::pair<T1, T2>& pair) const
			{
				return argument1 < std::get<0>(pair);
			}
			
			bool operator()(const std::pair<T1, T2>& pair, const T1& argument1) const
			{
				return std::get<0>(pair) < argument1;
			}
			
			bool operator()(const T2& argument2, const std::pair<T1, T2>& pair) const
			{
				return argument2 < std::get<1>(pair);
			}
			
			bool operator()(const std::pair<T1, T2>& pair, const T2& argument2) const
			{
				return std::get<1>(pair) < argument2;
			}
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using internal_set = std::set<std::pair<T1, T2>, comparator>;
		using iterator = typename internal_set::iterator;
		using const_iterator = typename internal_set::const_iterator;
		using reverse_iterator = typename internal_set::reverse_iterator;
		using const_reverse_iterator = typename internal_set::const_reverse_iterator;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		bimap()
		:
			_default_at(_set.cend())
		{
			
		}

		bimap(const std::initializer_list<typename internal_set::value_type> list)
		:
			_set(list),
			_default_at(_set.cend())
		{
			
		}
		
		bimap(const std::initializer_list<typename internal_set::value_type> list, const T1& key)
		:
			_set(list),
			_default_at(_set.find(key))
		{
			
		}
		
		bimap(const std::initializer_list<typename internal_set::value_type> list, const T2& key)
		:
			_set(list),
			_default_at(_set.find(key))
		{

		}
		
		bimap(bimap&& origin)
		:
			_set(std::move(origin._set)),
			_default_at(std::move(_set.default_at))
		{
			
		}
		
		bimap(const bimap& origin)
		:
			_set(origin._set),
			_default_at(origin._default_at)
		{
			
		}
		
		~bimap() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		internal_set _set;
		const_iterator _default_at;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		void emplace(T1&& argument1, T2&& argument2)
		{
			_set.emplace(std::make_pair(std::forward<T1>(argument1), std::forward<T2>(argument2)));
		}
		
		void insert(const T1& argument1, const T2& argument2)
		{
			_set.insert(std::make_pair(argument1, argument2));
		}
		
		template<typename T>
		typename internal_set::size_type erase(const T& key)
		{
			return _set.erase(key);
		}
		
		template<typename T>
		void erase(const T& key)
		{
			const_iterator i(_set.find(key));
			
			if (_set.cend() != i) {
				_set.erase(i);
			}
		}
		
		void clear() noexcept
		{
			_set.clear();
		}
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		const T2& operator[](const T1& key) noexcept
		{
			const_iterator i(_set.find(key));
			
			if (_set.cend() != i) {
				std::tie(i, std::ignore) = _set.emplace(std::make_pair(key, T2()));
			}
			
			return i->second;
		}
		
		const T1& operator[](const T2& key) noexcept
		{
			const_iterator i(_set.find(key));
			
			if (_set.cend() != i) {
				std::tie(i, std::ignore) = _set.emplace(std::make_pair(T1(), key));
			}
			
			return i->first;
		}
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const T2& at(const T1& key, bool use_default = false) const
		{
			const const_iterator& i(_set.find(key));
			if (_set.end() == i) {
				if (!use_default || _set.cend() == _default_at) {
					throw std::out_of_range("biset::at key not found");
				}
				
				return _default_at->second;
			}
			
			return i->second;
		}
		
		const T1& at(const T2& key, bool use_default = false) const
		{
			const const_iterator& i(_set.find(key));
			if (_set.end() == i) {
				if (!use_default || _set.cend() == _default_at) {
					throw std::out_of_range("biset::at key not found");
				}
				
				return _default_at->first;
			}
			
			return i->first;
		}
		
		T2& at(const T1& key, bool use_default = false)
		{
			const iterator& i(_set.find(key));
			if (_set.end() == i) {
				if (!use_default || _set.cend() == _default_at) {
					throw std::out_of_range("biset::at key not found");
				}
				
				return _default_at->second;
			}
			
			return i->second;
		}
		
		T1& at(const T2& key, bool use_default = false)
		{
			const iterator& i(_set.find(key));
			if (_set.end() == i) {
				if (!use_default || _set.cend() == _default_at) {
					throw std::out_of_range("biset::at key not found");
				}
				
				return _default_at->first;
			}
			
			return i->first;
		}

		typename internal_set::size_type size() const noexcept
		{
			return _set.size();
		}
		
		typename internal_set::size_type count(const T1& key) const noexcept
		{
			return _set.count(key);
		}
		
		typename internal_set::size_type count(const T2& key) const noexcept
		{
			return _set.count(key);
		}
		
		const_iterator find(const T1& key) const noexcept
		{
			return _set.find(key);
		}
		
		const_iterator find(const T2& key) const noexcept
		{
			return _set.find(key);
		}

		iterator begin() noexcept
		{
			return _set.begin();
		}
		
		iterator end() noexcept
		{
			return _set.end();
		}
		
		iterator begin() const noexcept
		{
			return _set.begin();
		}
		
		iterator end() const noexcept
		{
			return _set.end();
		}
		
		const_iterator cbegin() const noexcept
		{
			return _set.cbegin();
		}
		
		const_iterator cend() const noexcept
		{
			return _set.cend();
		}
		
		reverse_iterator rbegin() noexcept
		{
			return _set.rbegin();
		}
		
		reverse_iterator rend() noexcept
		{
			return _set.rend();
		}
		
		const_reverse_iterator crbegin() const noexcept
		{
			return _set.crbegin();
		}
		
		const_reverse_iterator crend() const noexcept
		{
			return _set.crend();
		}
	/** @} */
		
	/** @name States  */
	/** @{ */
	public:		
		bool empty() const noexcept
		{
			return _set.empty();
		}
		
		bool has(const T1& key) const noexcept
		{
			return find(key) != _set.end();
		}
		
		bool has(const T2& key) const noexcept
		{
			return find(key) != _set.end();
		}
	/** @} */
	};
}

#endif
