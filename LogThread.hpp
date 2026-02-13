/**
 @file LogThread.hpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_LogThread_hpp
#define Chaos_LogThread_hpp

#include <thread>
#include <atomic>
#include <memory>
#include <set>

namespace chaos {
	class log;

	class log_thread
	{
	/** @name Statics */
	/** @{ */
	public:
		static std::shared_ptr<log_thread> shared_instance();
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		log_thread();
		log_thread(log_thread&&) = delete;
		log_thread(const log_thread&) = delete;
		~log_thread();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::atomic<bool> _idle;
		std::thread _thread;
		std::set<std::shared_ptr<log>> _set;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		log_thread& operator=(log_thread&&) = delete;
		log_thread& operator=(const log_thread&) = delete;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		bool start();
		bool stop();
		bool list(const std::shared_ptr<log>& value);
		bool unlist(const std::shared_ptr<log>& value);

	private:
		void loop();
	/** @} */
	};
}

#endif
