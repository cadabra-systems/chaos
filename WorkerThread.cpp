/**
 @file WorkerThread.cpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "WorkerThread.hpp"

#include "String.hpp"
#include "LogRegister.hpp"
#include "Log.hpp"

#include <utility>
#include <stdexcept>
#include <exception>

namespace chaos {
	std::shared_ptr<worker_thread> worker_thread::instance()
	{
		std::shared_ptr<worker_thread> instance(new worker_thread());
		instance->_thread = std::thread(&worker_thread::loop, instance.get());
		
		return instance;
	}

	worker_thread::worker_thread()
	:
		_done(true)
	{

	}

	worker_thread::~worker_thread()
	{
		stop();
	}

	void worker_thread::send(std::function<void()> msg)
	{
		_queue.enqueue(msg);
	}

	void worker_thread::loop()
	{
		if (!_done.exchange(false)) {
			return ;
		}
		
		std::size_t exception_hash(0);
		std::uint64_t exception_counter(0);
		while (!_done) {
			std::function<void()> f;
			_queue.wait_dequeue(f);
			try {
				f();
				exception_hash = 0;
				exception_counter = 0;
			} catch (const std::exception& e) {
				log_register<log>::exception(e, exception_hash, exception_counter);
			} catch (...) {
				log_register<log>::exception(std::bad_exception(), exception_hash, exception_counter);
			}
		}
	}

	bool worker_thread::run()
	{
		if (_done) {
			_thread = std::thread(&worker_thread::loop, this);
			return _thread.joinable();
		}

		return false;
	}
	
	bool worker_thread::stop()
	{
		if (_done) {
			return false;
		}
		_queue.enqueue(std::bind(&worker_thread::done, this));
		if (!_thread.joinable()) {
			return false;
		}
		_thread.join();
		return true;
	}
	
	void worker_thread::done()
	{
		_done = true;
	}
	
	bool worker_thread::is_busy() const
	{
		return (_done) ? false : _thread.joinable();
	}
}
