/**
 @file WorkerPool.hpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_WorkerPool_hpp
#define Chaos_WorkerPool_hpp

#include "BlockingQueue.hpp"
#include "Delegate.hpp"
#include "Log.hpp"

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <thread>

namespace chaos {
	class worker_pool
	{
	/** @name Classes */
	/** @{ */
	public:
		class abstract_routine
		{
		/** @name Constructors */
		/** @{ */
		public:
			abstract_routine() = default;
			abstract_routine(const abstract_routine&) = delete;
			abstract_routine(abstract_routine&&) = delete;
			virtual ~abstract_routine() = default;
		/** @} */

		/** @name Operators */
		/** @{ */
		public:
			abstract_routine& operator=(const abstract_routine&) = delete;
			abstract_routine& operator=(abstract_routine&&) = delete;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void work() = 0;
		/** @} */
		};

		class lambda : public abstract_routine
		{
		/** @name Aliases */
		/** @{ */
		public:
			using function = std::function<void()>;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			explicit lambda(function&& function)
			:
				_function(std::move(function))
			{
			}
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			function _function;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual void work() override
			{
				if (_function) {
					_function();
				}
			}
		/** @} */
		};

		class probe : public std::enable_shared_from_this<probe>
		{
		/** @name Constructors */
		/** @{ */
		public:
			probe() = default;
			virtual ~probe() = default;
		/** @} */

		/** @name Procedures */
		/** @{ */
		public:
			virtual bool test() = 0;
		/** @} */
		};

		class worker
		{
		/** @name Constructors */
		/** @{ */
		public:
			worker(worker_pool* pool = nullptr)
			:
				_pool(pool)
			{
				_thread = std::thread(&worker::loop, this);
			}

			worker(const worker&) = delete;

			~worker()
			{
				join();
			}
		/** @} */

		/** @name Operators */
		/** @{ */
		public:
			worker& operator=(const worker&) = delete;
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			worker_pool* _pool;
			std::thread _thread;
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			void loop()
			{
				if (!_pool) {
					return;
				}

				consumer_token queue_token(_pool->queue());
				if (!_pool->is_ready()) {
					return;
				}

				std::size_t exception_hash(0);
				std::uint64_t exception_counter(0);
				do {
					std::shared_ptr<abstract_routine> routine;
					_pool->dequeue(queue_token, routine);
					if (routine) {
						try {
							routine->work();
							exception_hash = 0;
							exception_counter = 0;
						} catch (const std::exception& e) {
							log_register<log>::exception(e, exception_hash, exception_counter);
						} catch (...) {
							log_register<log>::exception(std::bad_exception(), exception_hash, exception_counter);
						}
					}
				} while (_pool->is_vacuum());
			}

		public:
			void join()
			{
				if (_thread.joinable()) {
					_thread.join();
				}
			}
		/** @} */

		/** @name Getters */
		/** @{ */
		public:
			std::thread::id get_id() const
			{
				return _thread.get_id();
			}
		/** @} */
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using conservator = std::function<bool()>;
		using container = std::list<worker>;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static std::size_t capacity()
		{
#ifdef WIN32
			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			return sysinfo.dwNumberOfProcessors;
#elif MACOS
			int nm[2];
			size_t length(4);
			uint32_t count;

			nm[0] = CTL_HW;
			nm[1] = HW_AVAILCPU;
			sysctl(nm, 2, &count, &length, nullptr, 0);

			if (count < 1) {
				nm[1] = HW_NCPU;
				sysctl(nm, 2, &count, &length, nullptr, 0);
				if (count < 1) {
					count = 1;
				}
			}
			return count;
#else
			return sysconf(_SC_NPROCESSORS_ONLN);
#endif
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		worker_pool()
		:
			_mother_thread_id(std::this_thread::get_id()),
			_pump(_promise.get_future().share()),
			_vacuum(false),
			_queue_token(_queue)
		{
		}

		~worker_pool()
		{
			_vacuum.exchange(false, std::memory_order_release);
			if (_pump.valid() && _pump.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
				_promise.set_value(false);
			}
			blocking_atomic_queue<std::shared_ptr<abstract_routine>>& queue(_queue);
			producer_token& queue_token(_queue_token);
			std::for_each
			(
				_list.cbegin(), _list.cend(),
				[&queue, &queue_token](const worker&)
				{
					queue.enqueue(queue_token, std::shared_ptr<abstract_routine>{});
				}
			);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::thread::id _mother_thread_id;
		delegate<bool()> _health_checker;
		std::promise<bool> _promise;
		std::shared_future<bool> _pump;
		std::atomic<bool> _vacuum;
		blocking_atomic_queue<std::shared_ptr<abstract_routine>> _queue;
		producer_token _queue_token;
		std::list<worker> _list;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		std::thread::id spawn()
		{
			return (_vacuum.load() || _mother_thread_id != std::this_thread::get_id())
			? std::thread::id()
			: _list.emplace_back(this).get_id()
			;
		}

		bool operator()(std::shared_ptr<abstract_routine> routine)
		{
			return (_mother_thread_id != std::this_thread::get_id())
			? false
			: _queue.enqueue(_queue_token, std::move(routine))
			;
		}

		bool operator()(producer_token& token, std::shared_ptr<abstract_routine> routine)
		{
			return _queue.enqueue(token, std::move(routine));
		}

		bool operator()(lambda::function&& function)
		{
			return (*this)(std::make_shared<lambda>(std::move(function)));
		}

		bool operator()(producer_token& token, lambda::function&& function)
		{
			return (*this)(token, std::make_shared<lambda>(std::move(function)));
		}

		bool dequeue(consumer_token& token, std::shared_ptr<abstract_routine>& routine)
		{
			return _queue.wait_dequeue(token, routine), true;
		}

		bool vacuum()
		{
			return _vacuum.exchange(true) ? false : true;
		}

		conservator preserve()
		{
			if (_vacuum.exchange(true, std::memory_order_release)) {
				return conservator
				(
					[]() -> bool
					{
						return false;
					}
				);
			}
			_promise.set_value(true);
			std::atomic<bool>& onoff(_vacuum);
			return conservator
			(
				[&onoff]() -> bool
				{
					return onoff.exchange(false, std::memory_order_release);
				}
			);
		}

		void check_health() const
		{
			_health_checker();
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		blocking_atomic_queue<std::shared_ptr<abstract_routine>>& queue()
		{
			return _queue;
		}

		const std::thread::id& mother_id() const
		{
			return _mother_thread_id;
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void health_checker(std::function<bool()> delegate)
		{
			if (!_vacuum.load() && _mother_thread_id == std::this_thread::get_id()) {
				_health_checker.add(std::move(delegate));
			}
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_vacuum() const
		{
			return _vacuum.load(std::memory_order_acquire);
		}

		bool is_ready() const
		{
			return _pump.get();
		}

		bool is_empty() const
		{
			return _list.empty();
		}
	/** @} */
	};
}

#endif
