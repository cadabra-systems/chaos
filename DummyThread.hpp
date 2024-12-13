/**
 @file DummyThread.hpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_DummyThread_hpp
#define Chaos_DummyThread_hpp

#include "WorkerThread.hpp"

namespace chaos {
	class dummy_thread : public worker_thread
	{
	/** @name Factories */
	/** @{ */
	public:
		static std::shared_ptr<dummy_thread> instance();
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		dummy_thread();
		dummy_thread(const dummy_thread&) = delete;
		dummy_thread& operator=(const dummy_thread&) = delete;
		
	public:
		virtual ~dummy_thread();
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual void send(std::function<void()> msg) override;
	/** @} */
	};
}

#endif
