/**
 @file WorkerThread.hpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_WorkerThread_hpp
#define Chaos_WorkerThread_hpp

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <memory>

//#include "SharedQueue.hpp"
//#include "AtomicQueue.hpp"
#include "BlockingQueue.hpp"

namespace chaos {
	class worker_thread
	{
	/** @name Statics */
	/** @{ */
	public:
		static std::shared_ptr<worker_thread> instance();
	/** @} */

	/** @name Constructors */
	/** @{ */
	protected:
		worker_thread();
		worker_thread(const worker_thread&) = delete;
		worker_thread& operator=(const worker_thread&) = delete;
		
	public:
		virtual ~worker_thread();
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		blocking_atomic_queue<std::function<void()>> _queue;
		std::thread _thread;
		std::atomic<bool> _done;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	private:
		void loop();
		
	public:
		bool run();
		bool stop();
		virtual void send(std::function<void()> msg);

	private:
		void done();
		
	public:
		bool is_busy() const;
	/** @} */
	};
}

#endif
