/**
 @file AtomicMap.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_AtomicMap_hpp
#define Chaos_AtomicMap_hpp

#include <unordered_map>
#include <memory>
#include <cstdint>

#include "Hash.hpp"

namespace chaos {
	template <typename T>
	class atomic_map
	{
	/** @name Classes */
	/** @{ */
	private:
		class holder
		{
		/** @name Constructors */
		/** @{ */
		public:
			holder()
			:
				_busy(false),
				_value(nullptr)
			{
				
			};
			
			holder(std::shared_ptr<T> value)
			:
				_busy(nullptr == value ? false : true),
				_value(value)
			{
				
			};
			
			holder() = default;
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			std::atomic_bool _busy;
			std::shared_ptr<T> _value;
		/** @} */
			
		/** @name Procedures */
		/** @{ */
		public:
			bool operator()(std::shared_ptr<T> value)
			{
				if (_busy.exchange(true)) {
					return false;
				}
				
				_value = value;
				
				return true;
			};
			
			void operator()(std::nullptr_t dummy)
			{
				_busy.exchange(false);
				_value.reset();
			};
		/** @} */
			
		public:
			std::shared_ptr<T> share() const
			{
				return _value;
			};
		};
	/** @} */
		
	/** @name Aliases */
	/** @{ */
	public:
		using map_type = typename std::unordered_map<std::uintptr_t, holder>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		hash_table<TValue>() = default;
		hash_table<TValue>(const std::size_t capacity = 0, const std::size_t spare = 100)
		:
			_map(length),
			_spare(spare),
			_capacity(capacity)
		{
			if (_total_limit > 0) {
				_map.reserve(total_limit);
			}
			
			if (spare > 0) {
				expand(100);
			}
			
			_vacancy_iterator = _map.cbegin();
		}
		virtual ~hash_table() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		map_type _map;
		map_type::const_iterator _vacancy_iterator;
		const std::size_t _capacity;
		const std::size_t _spare;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		std::shared_ptr<TValue> get(const std::size_t hash) const
		{
			/// @todo mswr-lock (read)
			typename table_type::const_iterator i(_map.find(hash));

			return (_map.end() != i) ? i->second.share() : return nullptr;
		}

/*
		const_iterator begin() const
		{
			return _list.begin();
		}
		
		const_iterator end() const
		{
			return _list.end();
		}

		const_iterator find(const std::size_t id) const
		{
			return _list.find(_map.find(id));
		}

		const_iterator find(const std::shared_ptr<TValue> value) const
		{
			return find_if(_list.begin(), _list.end(), [&value](typename list_type::value_type const& i) { return (*i).second == value; });
		}
*/
	/** @} */
		
	/** @name Mutators */
	/** @{ */
	public:
		std::pair<const_iterator, bool> insert(std::shared_ptr<TValue> value)
		{
			if (nullptr == value) {
				return std::make_pair(_list.end(), false);
			}
			/// @xxx What if already exists?
			std::pair<typename table_type::const_iterator, bool> insertion(_map.insert(typename table_type::value_type(chaos::hash<TValue>()(value), value)));
			
			return std::make_pair(_list.insert(_list.end(), insertion.first), insertion.second);
		}

		void clear()
		{
			_map.clear();
			_vacancy_iterator = _map.cbegin();

		}
		
/*
		bool erase(const_iterator i)
		{
			_list.erase(i);
			_map.erase(i);

			/// @note Always succeeds (unless of course, it gets an invalid iterator, in which case the results are undefined)
			return true;
		}
*/

/*
		bool erase(const std::shared_ptr<TValue> value)
		{
			const_iterator i(_map.find(value));
			
			if (_map.end() != i) {
				_map.erase(i);
				
				return true;
			}

			return false;
		}
*/

		bool erase(const std::size_t hash)
		{
			const_iterator i(_map.find(hash));
			
			if (_list.end() == i) {
				return false;
			}
			
			_map.erase((*i));
			
			return true;
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool empty() const
		{
			/// @todo mswr-lock (read)
			return _map.size() <= 0 ? true : false;
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	private:
		std::size_t expand(std::size_t count)
		{
			std::size_t allow_count = _capacity <= 0 ? count : _capacity - _map.size();
			count = (count <= allow_count) ? count : allow_count;
			
			std::size_t i;
			for (i=0; i<count; ++i) {
				holder h;
				std::uintptr_t hash(reinterpret_cast<std::uintptr_t>(&h));
				_map.emplace(hash, std::move(holder));
			}
			
			return i;
		}
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		std::shared_ptr<TValue> operator()(const std::size_t hash)
		{
			/// @todo mswr-lock (read)
			return get(id);
		}
		
		std::shared_ptr<const TValue> operator()(const std::size_t hash) const
		{
			/// @todo mswr-lock (read)
			return get(id);
		}
	/** @} */
	};
}
#endif
