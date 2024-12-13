/**
 @file TaskThread.hpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_TaskThread_hpp
#define Chaos_TaskThread_hpp

#include "Task.hpp"
#include "BlockingQueue.hpp"
#include "WorkerPool.hpp"
#include "AtomicHashTable.hpp"
#include "LogRegister.hpp"
#include "Log.hpp"

#include <thread>
#include <chrono>
#include <memory>

namespace chaos {
	/**
	 * @brief Task Thread (Event Loop)
	 */
	template <typename T>
	class task_thread
	{
	/** @name Classes */
	/** @{ */
	public:
		class launcher
		{
		friend class task_thread;
		/** @name Constructors */
		/** @{ */
		public:
			launcher()
			:
				_task_thread(nullptr)
			{

			}
			virtual ~launcher() = default;
		/** @} */

		/** @name Properties */
		/** @{ */
		protected:
			task_thread<T>* _task_thread;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void defer(std::shared_ptr<task_package<T>>) = 0;
			virtual bool resolve(std::shared_ptr<task_package<T>> package)
			{
				const bool retval(_task_thread->process(package->_task));
				package->resolve(retval);
				return retval;
			}
			virtual void swap(const std::shared_ptr<task_package<T>>& package1, const std::shared_ptr<task_package<T>>& package2)
			{
				if (package1 && package2) {
					std::swap(package1->_task, package2->_task);
				}
			}
		/** @} */
		};

		class sync_launcher : public task_thread<T>::launcher
		{
		/** @name Constructors */
		/** @{ */
		public:
			virtual ~sync_launcher() override = default;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void defer(std::shared_ptr<task_package<T>> package) override
			{
				this->resolve(package);
			}
		/** @} */
		};

		class async_launcher : public task_thread<T>::launcher
		{
		/** @name Constructors */
		/** @{ */
		public:
			virtual ~async_launcher() override = default;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void defer(std::shared_ptr<task_package<T>> package) override
			{
				std::async(std::launch::async, &task_thread<T>::async_launcher::resolve, this, package);
			}
		/** @} */
		};

		class queue_launcher : public task_thread<T>::launcher
		{
		/** @name Constructors */
		/** @{ */
		public:
			queue_launcher()
			:
				task_thread<T>::launcher(),

				_queue_thread(&task_thread<T>::queue_launcher::loop, this)
			{

			}
			virtual ~queue_launcher() override
			{
				if (_queue_thread.joinable()) {
					_package_queue.enqueue(nullptr);
					_queue_thread.join();
				}
			}
		/** @} */

		/** @name Opeartors */
		/** @{ */
		private:
			blocking_atomic_queue<std::shared_ptr<task_package<T>>> _package_queue;
			std::thread _queue_thread;
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			void loop()
			{
				std::shared_ptr<task_package<T>> package;
				do {
					_package_queue.wait_dequeue(package);
					if (nullptr != package) {
						try {
							this->resolve(package);
						} catch (const std::exception& e) {
							package->resolve(false);
							log_register<chaos::log>::error("Uncaught exception in the task thread: ", e.what());
						}
					}
				} while (nullptr != package);
			}

		public:
			virtual void defer(std::shared_ptr<task_package<T>> package) override
			{
				_package_queue.enqueue(package);
			}
		/** @} */
		};

		class pool_launcher : public task_thread<T>::launcher
		{
		/** @name Constructors */
		/** @{ */
		public:
			pool_launcher(worker_pool& pool)
			:
				task_thread<T>::launcher(),

				_worker_pool(pool)
			{

			}
			virtual ~pool_launcher() override = default;
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			chaos::worker_pool& _worker_pool;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void defer(std::shared_ptr<task_package<T>> package) override
			{
				static thread_local chaos::producer_token producer_token(_worker_pool.queue());
				_worker_pool(producer_token, std::bind(&task_thread<T>::pool_launcher::resolve, this, package));
			}
		/** @} */
		};

		template <typename R>
		class routine_launcher : public task_thread<T>::launcher
		{
		/** @name Aliases */
		/** @{ */
		public:
			using BulkArray = std::array<std::shared_ptr<task_package<T>>, 100>;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			routine_launcher(worker_pool& pool, bool bulk_mode = false)
			:
				task_thread<T>::launcher(),

				_worker_pool(pool),
				_bulk_mode(bulk_mode)
			{

			}
			virtual ~routine_launcher() override = default;
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			chaos::worker_pool& _worker_pool;
			atomic_hash_table<std::shared_ptr<R>> _table;
			blocking_atomic_queue<std::shared_ptr<task_package<T>>> _queue;
			const bool _bulk_mode;
		/** @} */

		/** @name Procedures */
		/** @{ */
		protected:
			virtual std::shared_ptr<task_package<T>> bulk(typename BulkArray::iterator first, typename BulkArray::iterator last) const
			{
				return nullptr;
			}

		public:
			virtual void defer(std::shared_ptr<task_package<T>> package) override
			{
				static thread_local chaos::producer_token producer_token(_queue);
				_queue.enqueue(producer_token, package);
			}

			virtual bool resolve(std::shared_ptr<task_package<T>> package) override
			{
				if (nullptr != package) {
					return task_thread<T>::launcher::resolve(package);
				}
				bool retval(false);
				static thread_local chaos::consumer_token consumer_token(_queue);
				using TaskArray = std::array<std::shared_ptr<task_package<T>>, 100>;
				TaskArray task_array;
				std::size_t count(0);
				do {
					count = _queue.try_dequeue_bulk(consumer_token, task_array.begin(), task_array.size());
					retval |= (count > 0);
					if (_bulk_mode && count > 1) {
						const typename TaskArray::iterator t_end(std::next(task_array.begin(), count));
						const std::shared_ptr<task_package<T>> bulk_package(bulk(task_array.begin(), t_end));
						if (bulk_package) {
							count = 1;
							task_thread<T>::launcher::swap(*(task_array.begin()), bulk_package);
							std::for_each(
											std::next(task_array.begin()), t_end,
											[this](std::shared_ptr<task_package<T>>& package)
											{
												/// @todo better to resolve as bulked
												package->resolve(true);
											}
							);
						}
					}
					for (std::size_t p = 0; p < count; ++p) {
						const std::size_t key(std::hash<T>()(task_array.at(p)->_task));
						typename atomic_hash_table<std::shared_ptr<R>>::iterator r(_table.find(key));
						bool ok(true);
						if (_table.end() == r) {
							std::tie(r, ok) = _table.try_emplace(key, std::make_shared<R>());
						}
						if (ok) {
							r->second->pool(std::move(task_array.at(p)), _worker_pool, ok);
						}
					}
				} while (count > 0);
				return retval;
			}
		/** @} */
		};

		enum behaviour
		{
			serial, /// < sync
			parallel, /// < async
			loop, /// < queue
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using task = T;
		using task_vector = std::vector<std::reference_wrapper<T>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		task_thread(const std::string& name, std::unique_ptr<launcher> launcher)
		:
			_name(name),
			_launcher(std::move(launcher))
		{
			if (_launcher) {
				_launcher->_task_thread = this;
			}
		}

		task_thread(const std::string& name, behaviour launch)
		:
			_name(name)
		{
			switch (launch) {
				case behaviour::serial: _launcher.reset(new sync_launcher()); break;
				case behaviour::parallel: _launcher.reset(new async_launcher()); break;
				case behaviour::loop: _launcher.reset(new queue_launcher()); break;
				default: return ;
			}
			_launcher->_task_thread = this;
		}

		task_thread(const std::string& name, worker_pool& pool, bool deffered = false)
		:
			_name(name),
			_launcher(deffered ? new pool_launcher(pool) : new pool_launcher(pool))
		{
			if (_launcher) {
				_launcher->_task_thread = this;
			}
		}

		task_thread(task_thread&& origin)
		:
			_name(std::move(origin._name)),
			_launcher(std::move(origin._launcher))
		{

		}

		task_thread(const task_thread&) = delete;
		task_thread& operator=(const task_thread&) = delete;

		virtual ~task_thread() = default;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	private:
		const std::string _name;
		std::unique_ptr<launcher> _launcher;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		bool flush()
		{
			return _launcher->resolve(nullptr);
		}

		task_future<T> drop(std::shared_ptr<task_package<T>> package)
		{
			_launcher->defer(package);
			return package->share();
		}

		task_future<T> pack(T&& task)
		{
			return drop(std::make_shared<task_package<T>>(std::move(task)));
		}

	protected:
		virtual bool process(const task_vector& task_vector) const
		{
			bool retval(true);
			for (const typename task_vector::value_type& t : task_vector) {
				retval &= process(t);
			}
			return retval;
		}

		virtual bool process(task& task_single) const
		{
			return true;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};

	template <typename T>
	using task_launcher = typename task_thread<T>::launcher;

	template <typename T>
	using task_sync_launcher = typename task_thread<T>::sync_launcher;

	template <typename T>
	using task_async_launcher = typename task_thread<T>::async_launcher;

	template <typename T>
	using task_queue_launcher = typename task_thread<T>::queue_launcher;

	template <typename T>
	using task_pool_launcher = typename task_thread<T>::pool_launcher;

	template <typename T>
	using task_routine_launcher = typename task_thread<T>::routine_launcher;
}

#endif
