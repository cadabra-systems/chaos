/**
 @file LogThread.cpp
 @date 02.04.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "LogThread.hpp"

#include "Log.hpp"

#include <exception>

namespace chaos {
	std::shared_ptr<log_thread> log_thread::shared_instance()
	{
		std::shared_ptr<log_thread> instance(new log_thread());
		instance->_thread = std::thread(&log_thread::loop, instance.get());
		return instance;
	}

	log_thread::log_thread()
	:
		_idle(true)
	{

	}

	log_thread::~log_thread()
	{
		stop();
	}

	bool log_thread::start()
	{
		if (!_idle.load(std::memory_order_acquire)) {
			return false;
		}
		_thread = std::thread(&log_thread::loop, this);
		return _thread.joinable();
	}
	
	bool log_thread::stop()
	{
		bool idle(false);
		if (!_idle.compare_exchange_strong(idle, true, std::memory_order_release, std::memory_order_relaxed)) {
			return false;
		} else if (!_thread.joinable()) {
			return false;
		}
		_thread.join();
		return true;
	}

	bool log_thread::list(const std::shared_ptr<log>& value)
	{
		bool idle(true);
		if (!_idle.compare_exchange_strong(idle, false, std::memory_order_release, std::memory_order_relaxed)) {
			return false;
		}
		const bool retval(value->set_mode(log::mode::queue) && _set.emplace(value).second);
		return (_idle = true) && retval;
	}

	bool log_thread::unlist(const std::shared_ptr<log>& value)
	{
		bool idle(true);
		if (!_idle.compare_exchange_strong(idle, false, std::memory_order_release, std::memory_order_relaxed)) {
			return false;
		}
		const bool retval(value ? (_set.erase(value) > 0) : (!_set.empty() && (_set.clear(), true)));
		return (_idle = true) && retval;
	}

	void log_thread::loop()
	{
		bool idle(true);
		if (!_idle.compare_exchange_strong(idle, false, std::memory_order_release, std::memory_order_relaxed)) {
			return ;
		}
		std::size_t exception_hash(0);
		std::uint64_t exception_counter(0);
		do {
			try {
				for (const std::set<std::shared_ptr<log>>::value_type& log : _set) {
					while (log->flush() > 0) {
						/// @??? What if loads of...
					}
				}
				exception_hash = 0;
				exception_counter = 0;
			} catch (const std::exception& e) {
				log_register<log>::exception(e, exception_hash, exception_counter);
			} catch (...) {
				log_register<log>::exception(std::bad_exception(), exception_hash, exception_counter);
			}
		} while (!_idle.load(std::memory_order_acquire));
	}
}
