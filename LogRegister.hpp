/**
 @file LogRegister.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_LogRegister_hpp
#define Chaos_LogRegister_hpp

#include "LogThread.hpp"
#include "LogStream.hpp"
#include "LogFormatLevel.hpp"
#include "String.hpp"

#include <memory>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <exception>

namespace chaos {
	template <typename L>
	class log_register
	{
	/** @name Singletons */
	/** @{ */
	public:
		static log_register<L>& instance()
		{
			static log_register<L> singleton;
			return singleton;
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		log_register()
		:
			_log(new L())
		{

		}

		~log_register()
		{

		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::shared_ptr<L> _log;
		std::shared_ptr<log_thread> _thread;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		static bool thread(const std::shared_ptr<log_thread>& thread)
		{
			if (!thread || log_register<L>::instance()._thread) {
				return false;
			} else if (thread->list(log_register<L>::instance()._log)) {
				log_register<L>::instance()._thread = thread;
				return true;
			}
			return false;
		}

		static bool thread()
		{
			if (!log_register<L>::instance()._thread) {
				return false;
			} else if (log_register<L>::instance()._thread->unlist(log_register<L>::instance()._log)) {
				log_register<L>::instance()._thread.reset();
				return true;
			}
			return false;
		}

		static bool stream(std::ostream& stream, log_level level, log_format format = log_format::text)
		{
			return (level == log_level::none) ? false : log_register<L>::instance()._log->map(stream, level, format);
		}

		static bool stream(std::unique_ptr<log_stream>&& stream, log_level level)
		{
			return (level == log_level::none) ? false : log_register<L>::instance()._log->map(std::move(stream), level);
		}

		static void stream()
		{
			log_register<L>::instance()._log->unmap();
		}

		template <typename... Args>
		static void log(log_level level, Args&&... args)
		{
			std::string message(string::concatenate(std::forward<Args>(args)...));
			log_register<L>::instance()._log->add(std::move(level), std::move(message));
		}

		template <typename... Args>
		static void fatal(const Args&... args)
		{
			log(log_level::fatal, args...);
		}

		template <typename... Args>
		static void alert(const Args&... args)
		{
			log(log_level::alert, args...);
		}

		template <typename... Args>
		static void critical(const Args&... args)
		{
			log(log_level::critical, args...);
		}

		template <typename... Args>
		static void error(const Args&... args)
		{
			log(log_level::error, args...);
		}

		template <typename... Args>
		static void warning(const Args&... args)
		{
			log(log_level::warning, args...);
		}

		template <typename... Args>
		static void checkpoint(const Args&... args)
		{
			log(log_level::checkpoint, args...);
		}

		template <typename... Args>
		static void notice(const Args&... args)
		{
			log(log_level::notice, args...);
		}

		template <typename... Args>
		static void debug(const Args&... args)
		{
			log(log_level::debug, args...);
		}

		static void exception(const std::exception& e, std::size_t& hash, std::uint64_t& counter)
		{
			try {
				if (std::exchange(hash, std::hash<std::string>()(e.what())) != hash) {
					error("Uncaught exception(", hash, "): `", e.what(), "`. Just swallowing to keep this thread running...");
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
				exception(std::runtime_error(string::formatted_string("Same exception(`%s`) repeated more than 10k times in a row, give up counting this...", e.what(), counter)), dummy1, dummy2);
			} else if (10'000 == counter || 1'000 == counter || 100 == counter || 10 == counter) {
				exception(std::runtime_error(string::formatted_string("Same exception(`%s`) repeated %llu times in a row", e.what(), counter)), dummy1, dummy2);
			}
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
