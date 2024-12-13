/**
 @file Task.hpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Task_hpp
#define Chaos_Task_hpp

#include "Delegate.hpp"

#include <functional>
#include <future>
#include <atomic>

namespace chaos {
	template <typename T>
	class task_future
	{
	/** @name Constructors */
	/** @{ */
	public:
		task_future(std::shared_future<bool> future)
		:
			_future(future)
		{

		}
		task_future(task_future<T>&& origin)
		:
			_future(std::move(origin._future))
		{

		}
		task_future(const task_future<T>& origin)
		:
			_future(origin._future)
		{

		}
		~task_future() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_future<bool> _future;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		task_future<T>& operator=(task_future<T>&& rhs)
		{
			_future.swap(rhs._future);
		}

		task_future<T>& operator=(const task_future& rhs)
		{
			_future = rhs._future;
		}

		operator std::shared_future<bool>() const
		{
			return _future;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_resolved() const
		{
			return !_future.valid() ? true : std::future_status::ready == _future.wait_for(std::chrono::seconds(0));
		}

		bool is_successed() const
		{
			return !_future.valid() || !is_resolved() ? false : _future.get();
		}
	/** @} */
	};

	template <typename T>
	class task_package : public std::enable_shared_from_this<task_package<T>>
	{
	template <typename>
	friend class task_thread;
	template <typename>
	friend class task_launcher;
	/** @name Constructors */
	/** @{ */
	public:
		task_package(T&& task)
		:
			_task(task),
			_future(_promise.get_future())
		{

		}
		task_package(task_package&& origin)
		:
			_task(std::move(origin._task)),
			_promise(std::move(origin._promise)),
			_future(std::move(origin._future)),
			_delegate(std::move(origin._delegate))
		{

		}
		task_package(const task_package&) = delete;
		task_package() = default;
		~task_package() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		T _task;
		std::promise<bool> _promise;
		std::future<bool> _future;
		chaos::delegate<void(bool, const T&)> _delegate;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		task_future<T> share()
		{
			return task_future<T>(_future.share());
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const T& get() const
		{
			return _task;
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void resolve(bool success)
		{
			_promise.set_value(success);
			_delegate(success, _task);
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void callback(std::function<void(bool, const T&)> callee)
		{
			_delegate.add(callee);
		}
	/** @} */
	};

	template<typename T, typename ...TArgs>
	std::shared_ptr<task_package<T>> pack_task(TArgs&& ...args)
	{
		return std::make_shared<task_package<T>>(new task_package<T>(std::forward<TArgs>(args)... ));
	}
}

#endif
