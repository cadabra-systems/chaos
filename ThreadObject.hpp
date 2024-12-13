/**
 @fileThreadObject.hpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_ThreadObject_hpp
#define Chaos_ThreadObject_hpp

#include <thread>
#include <unordered_map>

namespace chaos {
	template<typename T>
	class thread_object
	{
	/** @name Static */
	/** @{ */
	public:
		static thread_object<T>& instance()
		{
			static thread_object<T> instance;
			
			return instance;
		}
	/** @} */
		
	/** @name Aliases */
	/** @{ */
	public:
		using container = std::unordered_map<std::thread::id, std::shared_ptr<T>>;
	/** @} */
		
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
	
	/** @name Constructors */
	/** @{ */
	private:
		thread_object()
		:
			_preservation(false)
		{
		}
		
		~thread_object()
		{
		}
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		container _map;
		std::atomic<bool> _preservation;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		bool preserve()
		{
			return _preservation.exchange(true);
		}
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
		bool set(std::shared_ptr<T> object, const std::thread::id thread_id = std::this_thread::get_id())
		{
			if (nullptr == object || _preservation.load()) {
				return false;
			}
			
			bool retval(false);
			std::tie(std::ignore, retval) = _map.try_emplace(thread_id, object);
			
			return retval;
		}
	/** @} */
	
	/** @name Getters */
	/** @{ */
	public:
		std::shared_ptr<T> get(const std::thread::id thread_id = std::this_thread::get_id()) const
		{
			typename container::const_iterator i(_map.find(thread_id));
			return (_map.cend() == i) ? nullptr : i->second;
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool exist(const std::thread::id thread_id = std::this_thread::get_id()) const
		{
			return _map.count(thread_id) > 0;
		}
	/** @} */
	};
}

#endif
