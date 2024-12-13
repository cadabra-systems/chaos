/**
 @file ObjectPool.hpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
 */

#ifndef Chaos_ObjectPool_hpp
#define Chaos_ObjectPool_hpp

#include "Delegate.hpp"

#include <memory>
#include <list>
#include <chrono>

namespace chaos {
	template<typename T>
	class object_pool
	{
	/** @name Static */
	/** @{ */
	public:
		static object_pool<T>& general()
		{
			static object_pool<T> instance;
			
			return instance;
		}
	/** @} */
		
	/** @name Classes */
	/** @{ */
	public:
		/**
		 * @brief Управляющая структура
		 */
		class domain
		{
		/** @name Constructors */
		/** @{ */
		public:
			domain(std::shared_ptr<T> instance)
			:
				_instance(instance),
				_lock(false)
			{
				
			}
			
			domain(const object_pool<T>::domain& origin) = delete;
			~domain() = default;
		/** @} */
			
		/** @name Operators */
		/** @{ */
		public:
			std::shared_ptr<T> operator()() const
			{
				return _instance;
			}
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			const std::shared_ptr<T> _instance;
			mutable std::atomic<bool> _lock;
		/** @} */
			
		/** @name Procedures  */
		/** @{ */
		public:
			bool lock() const
			{
				if (nullptr == _instance) {
					return false;
				}
				
				return !_lock.exchange(true);
			}
			
			bool unlock() const
			{
				if (_lock.exchange(false)) {
					return true;
				}
				
				return false;
			}
		/** @} */
			
		/** @name Getters  */
		/** @{ */
		public:
			std::shared_ptr<T> get() const
			{
				return _instance;
			}
		/** @} */
			
		/** @name States  */
		/** @{ */
		public:
			bool is() const
			{
				return nullptr != _instance;
			}
		/** @} */
		};
		
		class holder
		{
		friend class object_pool<T>;
			
		/** @name Aliases */
		/** @{ */
		public:
		/** @} */
			
		/** @name Constants */
		/** @{ */
		public:
		/** @} */
			
		/** @name Constructors */
		/** @{ */
		private:
			holder(domain* domain)
			:
				_domain(domain)
			{
			}
			
		public:
			holder()
			:
				_domain(nullptr)
			{
			}
			
		public:
			holder(const holder& o) = delete;
			holder(holder&& origin) noexcept
			:
				_domain(std::move(origin._domain))
			{
				_domain = nullptr;
			}
			
			~holder()
			{
				if (nullptr != _domain) {
					_domain->unlock();
				}
			}
		/** @} */
			
		/** @name Operators */
		/** @{ */
		private:
			holder& operator=(const holder&) = delete;
			
		public:
			holder& operator=(holder&& origin)
			{
				reset(origin._domain);
				origin.reset();
				
				return *this;
			}
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			domain* _domain;
		/** @} */
			
		/** @name Procedures  */
		/** @{ */
		public:
			void reset(domain* domain = nullptr)
			{
				if (nullptr != _domain) {
					_domain->unlock();
				}
				
				_domain = domain;
			}
		/** @} */
			
		/** @name Operators */
		/** @{ */
		public:
			std::shared_ptr<T> operator()() const
			{
				return get();
			}
			
			std::shared_ptr<T> operator->() const
			{
				return get();
			}
		/** @} */
			
		/** @name Getters  */
		/** @{ */
		public:
			std::shared_ptr<T> get() const
			{
				return _domain->get();
			}
		/** @} */
			
		/** @name States  */
		/** @{ */
		public:
			bool is() const
			{
				return nullptr == _domain ? false : _domain->is();
			}
		/** @} */
		};
		
		class locker
		{
		friend class object_pool<T>;
			
		/** @name Classes */
		/** @{ */
		public:
		/** @} */
			
		/** @name Constructors */
		/** @{ */
		private:
			locker(std::shared_ptr<T> instance)
			:
				_domain(new domain(instance))
			{

			}
			
		public:
			locker(const locker& o) = delete;
			locker(locker&& origin)
			:
				_domain(origin._domain)
			{
				origin._domain = nullptr;
			}
			
			~locker()
			{
				delete _domain;
			}
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			domain* _domain;
		/** @} */
			
		/** @name Procedures  */
		/** @{ */
		public:
			bool hold(holder& holder) const
			{
				if (!is()) {
					return false;
				}
				
				if (!_domain->lock()) {
					return false;
				}
				
				holder.reset(_domain);
				return true;
			}
		/** @} */
			
		/** @name States  */
		/** @{ */
		public:
			bool is() const
			{
				return nullptr == _domain ? false : _domain->is();
			}
		/** @} */
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		object_pool() = default;
		object_pool(const object_pool<T>& origin) = delete;
		~object_pool() = default;
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
		delegate<bool(const std::shared_ptr<T>&)> _reuse_delegate;
		std::list<locker> _list;
		std::atomic<bool> _vacuum;
	/** @} */
	
	/** @name Getters  */
	/** @{ */
	public:
		bool push(std::shared_ptr<T> object)
		{
			if (_vacuum.load() || nullptr == object) {
				return false;
			}

			_list.push_back(locker(object));
			
			return true;
		}
		
		bool erase(std::shared_ptr<T> object)
		{
			if (_vacuum.load() || nullptr == object) {
				return false;
			}
			
			/// @todo iterate over list and check equality
/*
			typename std::list<T>::const_iterator i(_list.find(object));
			if (_list.cend() != i) {
				_list.erase(i);
			}
*/
			return true;
		}
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		const holder acquire(const std::chrono::milliseconds timeout = std::chrono::milliseconds(2000)) const
		{
			std::chrono::system_clock::time_point start(std::chrono::system_clock::now());
			
			holder retval;
			typename std::list<locker>::const_iterator l(_list.cbegin());
			while (l != _list.cend()) {
				if ((l->hold(retval))) {
					_reuse_delegate(retval.get());
					return retval;
				}
				if (_list.cend() == ++l) {
					/// @xxx потенциально бесконечный цикл
					if (timeout.count() != 0) {
						if (std::chrono::system_clock::now() - start >= timeout) {
							return retval;
						}
					}
					l = _list.cbegin();
				}
			}
			return retval;
		}
		
		bool vacuum()
		{
			if (_vacuum.exchange(true)) {
				return false;
			}
			return true;
		}

		std::shared_ptr<T> tie()
		{
			using namespace std::chrono_literals;
			static thread_local holder h(acquire(20000ms)); /// @xxx
			return h.get();
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void health_checker(std::function<bool(const std::shared_ptr<T>&)> delegate)
		{
			if (!_vacuum.load()) {
				_reuse_delegate.add(std::move(delegate));
			}
		}
	/** @} */

	/** @name States  */
	/** @{ */
	public:
		bool empty() const
		{
			return _list.empty();
		}
	/** @} */
	};
}

#endif
