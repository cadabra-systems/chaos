/**
 @file ThreadPool.hpp
 @date 14.03.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_ThreadPool_hpp
#define Chaos_ThreadPool_hpp

#include "MarkedPointer.hpp"
#include "Delegate.hpp"

#include <thread>
#include <list>
#include <condition_variable>

namespace chaos {
	/**
	 * @brief Thread Pool
	 * @deprecated in favor of worker_pool
	 */
	class thread_pool
	{
	/** @name Classes */
	/** @{ */
	public:
		class thread
		{
		/** @name Aliases */
		/** @{ */
		public:
			using lambda = std::function<void()>;
			using marked_lambda = marked_ptr<lambda>;
			using atomic_lambda = std::atomic<lambda*>;
		/** @} */

		/** @name Statics */
		/** @{ */
		private:
			static const std::uint8_t idle = 0b00;
			static const std::uint8_t ready = 0b01;
			static const std::uint8_t busy = 0b10;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			thread(thread_pool* pool = nullptr)
			:
				_pool(pool),
				_infinity(true),
				_lambda(nullptr)
			{
				_thread = std::thread(&thread::loop, this);
			}

			thread(const thread&) = delete;
			
			~thread()
			{
				join(true);
			}
		/** @} */
			
		/** @name Operators */
		/** @{ */
		public:
			thread& operator=(const thread&) = delete;
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			thread_pool* _pool;
			std::atomic<bool> _infinity;
			std::condition_variable _signal;
			std::thread _thread;
			atomic_lambda _lambda;
		/** @} */
			
		/** @name Procedures */
		/** @{ */
		public:
			bool co(lambda* t)
			{
				lambda* awaiter(nullptr);
				if (_lambda.compare_exchange_strong(awaiter, marked_lambda(t, thread::ready).all(), std::memory_order_acq_rel)) {
					do {
						_signal.notify_one();
					} while (marked_lambda(_lambda.load(std::memory_order_acquire)).mark() != thread::idle);
					return true;
				}
				return false;
			}

		private:
			void loop()
			{
				if (!_pool) {
					return ;
				}

				std::mutex mutex;
				std::unique_lock<std::mutex> lock(mutex);
				while (_infinity.load()) {
					marked_lambda t(_lambda.load(std::memory_order_acquire));
					while (t.mark() != thread::ready) {
						_signal.wait(lock);
						t = _lambda.load(std::memory_order_acquire);
					}
					_lambda.store((t |= thread::busy).all(), std::memory_order_release);
					t->operator()();
					t.dispose();
					_lambda.store(nullptr, std::memory_order_release);
				}
			}
			
		public:
			void join(bool stop = false)
			{
				if (stop) {
					_infinity.store(false);
					std::unique_ptr<std::function<void()>> stopper(new std::function<void()>([]() -> void { return ; }));
					co(stopper.get());
					stopper.release();
				}
				
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
		using container = std::list<thread>;
	/** @} */
		
	/** @name Statics */
	/** @{ */
	public:
		static unsigned int default_capacity()
		{
			/// @note may return 0 when not able to detect
			return std::thread::hardware_concurrency();
		}
	/** @} */
	
	/** @name Constructors */
	/** @{ */
	public:
		thread_pool()
		:
			_mother_thread_id(std::this_thread::get_id()),
			_vacuum(false),
			_infinity(true)
		{
			_w = _list.end();
		}
		
		~thread_pool()
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
		const std::thread::id _mother_thread_id;
		delegate<bool()> _health_checker;
		std::atomic<bool> _vacuum;
		std::atomic<bool> _infinity;
		std::list<thread> _list;
		typename container::iterator _w;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		std::thread::id spawn()
		{
			if (_vacuum.load() || _mother_thread_id != std::this_thread::get_id()) {
				return std::thread::id();
			}
			_list.emplace_back(this);
			
			return _list.back().get_id();
		}
		
		std::thread::id pop()
		{
			if (_list.empty() || _vacuum.load() || _mother_thread_id != std::this_thread::get_id()) {
				return std::thread::id();
			}
			
			std::thread::id retval(_list.back().get_id());
			_list.pop_back();
			
			return retval;
		}
		
		bool operator()(std::unique_ptr<chaos::thread_pool::thread::lambda>&& lambda)
		{
			if (_mother_thread_id != std::this_thread::get_id()) {
				return false;
			} else if (!_infinity.load()) {
				return false;
			}

			do {
				if (_list.end() == _w) {
					if (!_infinity.load())  {
						return false;
					}
					_w = _list.begin();
				}
			} while (!_w++->co(lambda.get()));
			lambda.release();
			return true;
		}
		
		void on()
		{
			_infinity.store(true);
		}
		
		void off()
		{
			_infinity.store(false);
		}
		
		conservator preserve()
		{
			if (_vacuum.exchange(true)) {
				return conservator(
									[]() -> bool
									{
										return false;
									}
				);
			}

			std::atomic<bool>& onoff(_vacuum);
			_w = _list.begin();

			return conservator(
								[&onoff]() -> bool
								{
									return onoff.exchange(false);
								}
			);
		}

		void check_health() const
		{
			_health_checker();
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
	
	/** @name Getters */
	/** @{ */
	public:
		const std::thread::id& mother_id() const
		{
			return _mother_thread_id;
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
