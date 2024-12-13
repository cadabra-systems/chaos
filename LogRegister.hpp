/**
 @file LogRegister.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_LogRegister_hpp
#define Chaos_LogRegister_hpp

#include "LogStream.hpp"
#include "LogFormatLevel.hpp"
#include "WorkerThread.hpp"
#include "DummyThread.hpp"
#include "String.hpp"

#include <future>
#include <iostream>
#include <memory>
#include <utility>
#include <stdexcept>
#include <exception>

namespace chaos {
	template <typename L>
	class log_register
	{
	/** @name Constructors */
	/** @{ */
	private:
		log_register() = default;
		virtual ~log_register() = default;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static void exception(const std::exception& e, std::size_t& hash, std::uint64_t& counter)
		{
			try {
				if (std::exchange(hash, std::hash<std::string>()(e.what())) != hash) {
					error("Uncaught exception(", hash, "): `", e.what(), "`. Just swallowig to keep this thread running...");
					counter = 1;
					return ;
				} else {
					++counter;
				}
			} catch (...) {
				if (std::exchange(hash, 1) != 1) {
					std::cerr << "Nested exception during catch&log of well-known exception: `" << e.what() << "`" << std::endl;
					counter = 1;
					return ;
				} else {
					++counter;
				}
			}
			static std::size_t dummy1(0); static std::uint64_t dummy2(0);
			if (10'000 < counter) {
				exception(std::runtime_error(string::formatted_string("Same exception(`%s`) repeated more than 10k times in a row, give up to count this ...", e.what(), counter)), dummy1, dummy2);
			} else if (10'000 == counter || 1'000 == counter || 100 == counter || 10 == counter) {
				exception(std::runtime_error(string::formatted_string("Same exception(`%s`) repeated %llu times in a row", e.what(), counter)), dummy1, dummy2);
			}
		}
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		static log_register<L>& instance()
		{
			static log_register<L> instance;
			
			return instance;
		}
		
		static std::shared_ptr<worker_thread> worker(std::shared_ptr<worker_thread> worker = nullptr)
		{
			static std::shared_ptr<worker_thread> retval(nullptr == worker ? dummy_thread::instance() : worker);
			
			return retval;
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		L _log;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
		bool add_stream(std::ostream& stream, log_level level, log_format format = log_format::text)
		{
			return (level == log_level::none) ? false : _log.add(stream, level, format);
		}

		bool add_stream(std::unique_ptr<log_stream>&& stream, log_level level)
		{
			return (level == log_level::none) ? false : _log.add(std::move(stream), level);
		}

		void remove_all_streams()
		{
			_log.clear();
		}
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
	/** @} */

	/** @name Templates */
	/** @{ */
	public:
		template<class... Args>
		static void log(const log_level level, const Args&... args)
		{
			std::thread::id thread_id(std::this_thread::get_id());
			log_register<L>::worker()->send(
											[=]()
											{
												log_register<L>& instance(log_register<L>::instance());
												instance._log(level, thread_id, args...);
											}
			);
		}
		
		static void flush()
		{
			log_register<L>::worker()->stop();
			log_register<L>::instance().remove_all_streams();
		}
		
		template<class... Args>
		static void fatal(const Args&... args)
		{
			log(log_level::fatal, args...);
		}
		
		template<class... Args>
		static void alert(const Args&... args)
		{
			log(log_level::alert, args...);
		}

		template<class... Args>
		static void critical(const Args&... args)
		{
			log(log_level::critical, args...);
		}

		template<class... Args>
		static void error(const Args&... args)
		{
			log(log_level::error, args...);
		}
		
		template<class... Args>
		static void warning(const Args&... args)
		{
			log(log_level::warning, args...);
		}

		template<class... Args>
		static void checkpoint(const Args&... args)
		{
			log(log_level::checkpoint, args...);
		}
		
		template<class... Args>
		static void notice(const Args&... args)
		{
			log(log_level::notice, args...);
		}
		
		template<class... Args>
		static void debug(const Args&... args)
		{
			log(log_level::debug, args...);
		}
	/** @} */
	};
}

#endif
