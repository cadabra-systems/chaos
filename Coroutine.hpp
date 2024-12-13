/**
 @file Coroutine.hpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Coroutine_hpp
#define Chaos_Coroutine_hpp

#include "../Chaos/ThreadPool.hpp"
#include "../Chaos/WorkerPool.hpp"

#include <atomic>
#include <memory>
#include <future>

namespace chaos {
	enum class coruntime
	{
		sync,
		async,
		task_pool,
		thread_pool,
		worker_pool
	};

	struct conroutinetext
	{
		coruntime runtime;
		void* pool;
	};

	template<typename T, typename R>
	class coroutine : public std::enable_shared_from_this<coroutine<T, R>>
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Aliases */
	/** @{ */
	public:
		using runtime = coruntime;
		using context = conroutinetext;
	/** @} */
		
	/** @name Constants */
	/** @{ */
	public:
	/** @} */
	
	/** @name Constructors */
	/** @{ */
	public:
		coroutine(T dummy_value)
		:
			_idle(true)
		{
			std::promise<T> dummy;
			_fuse = dummy.get_future();
			dummy.set_value(dummy_value);
		}
		
		virtual ~coroutine() = default;
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
		std::atomic_bool _idle;
		std::shared_future<T> _fuse;
	protected:
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual std::shared_future<T> async(R routine) final
		{
			std::shared_future<T> fuse(_fuse);
			return (_fuse = std::async(std::launch::async, &coroutine::then, this->shared_from_this(), context{coruntime::async, nullptr}, routine, fuse));
		}

		virtual std::shared_future<T> sync(R routine) final
		{
			std::shared_future<T> fuse(_fuse);
			return (_fuse = std::async(std::launch::deferred, &coroutine::then, this->shared_from_this(), context{coruntime::sync, nullptr}, routine, fuse));
		}

		virtual std::shared_future<T> pool(R routine, chaos::thread_pool& pool, bool& pool_success) final
		{
			std::shared_future<T> fuse(_fuse);
			_fuse = std::async(std::launch::deferred, &coroutine::then, this->shared_from_this(), context{coruntime::thread_pool, &pool}, routine, fuse);

			pool_success = pool(
									std::make_unique<chaos::thread_pool::thread::lambda>
									(
										[f = _fuse]() -> void
										{
											f.get();
											return ;
										}
									)
			);
			return _fuse;
		}
		
		virtual std::shared_future<T> pool(R routine, chaos::worker_pool& pool, bool& pool_success) final
		{
			std::shared_future<T> fuse(_fuse);
			_fuse = std::async(std::launch::deferred, &coroutine::then, this->shared_from_this(), conroutinetext{coruntime::worker_pool, &pool}, routine, fuse);

			pool_success = pool(
									[f = _fuse]() -> void
									{
										f.get();
										return ;
									}
			);
			return _fuse;
		}

		virtual void wait()
		{
			_fuse.wait();
		}
		
	protected:
		virtual T co(R routine, const context& context) = 0;
		
	private:
		virtual T then(context context, R routine, std::shared_future<T> fuse) final
		{
			fuse.wait();

			_idle = false;
			T retval(co(routine, context));
			_idle = true;

			return retval;
		}
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
	
	/** @name Getters */
	/** @{ */
	public:
		std::shared_future<T> future()
		{
			return _fuse;
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool idle() const
		{
			return _idle;
		}
	/** @} */
	};
}

#endif
