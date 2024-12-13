/**
 @file LogStream.hpp
 @date 24.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_LogStream_hpp
#define Chaos_LogStream_hpp

#include "Flex.hpp"
#include "ProgramName.hpp"
#include "LogFormatLevel.hpp"

#include <string>
#include <thread>
#include <ctime>
#include <iostream>
#include <sstream>
#include <syslog.h>

namespace chaos {
	class log_stream
	{
	/** @name Constants */
	/** @{ */
	public:
		static const std::map<log_level, std::string> level_map;
		static const std::hash<std::thread::id> thread_hasher;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		static std::string stamp_time();
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		log_stream();
		virtual ~log_stream() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::uint8_t _fail_counter;
		std::chrono::time_point<std::chrono::steady_clock> _suspend_timepoint;
	/** @} */

	/** @name Operators */
	/** @{ */
	private:
		template<typename A, typename... Args>
		void accumulate(A arg, const Args&... args)
		{
			get_accumulator() << arg;
			accumulate(args...);
		}

	protected:
		virtual void accumulate();
		virtual bool flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id) = 0;

	public:
		bool probe(const std::string& prefix);
		template<class... Args>
		void accumulate(const std::string& prefix, const log_level& level, const std::thread::id& thread_id, const Args&... args)
		{
			if (_fail_counter > 5 && _suspend_timepoint >= std::chrono::steady_clock::now()) {
				return ;
			}

			accumulate(args...);
			if (!flush(prefix, level, thread_id)) {
				if (++_fail_counter > 5) {
					std::size_t timeout(10 * _fail_counter);
					_suspend_timepoint = std::chrono::steady_clock::now() + std::chrono::seconds(timeout);
					std::cerr << "Log stream is not available and will be suspended for " << timeout << " seconds" << std::endl;
					get_accumulator().clear();
					get_accumulator() << "Log stream is not available and will be suspended for " << timeout << " seconds";
					flush("Log", log_level::error, thread_id);
				}
			} else if (_fail_counter > 0) {
				_fail_counter = 0;
				get_accumulator().clear();
				get_accumulator() << "Log stream now is back but some messages were probably lost";
				flush("Log", log_level::error, thread_id);
			}
			get_accumulator().clear();
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	protected:
		virtual std::iostream& get_accumulator() = 0;
	/** @} */
	};

	class text_log_stream : public log_stream
	{
	/** @name Constants */
	/** @{ */
	public:
		static const std::map<log_level, std::string> level_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		text_log_stream(std::ostream& target);
		virtual ~text_log_stream() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::ostream& _target;
		std::stringstream _accumulator;
	/** @} */

	/** @name Operators */
	/** @{ */
	protected:
		virtual void accumulate() override;
		virtual bool flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id) override;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		virtual std::iostream& get_accumulator() override;
	/** @} */
	};

	class csv_log_stream : public log_stream
	{
	/** @name Constants */
	/** @{ */
	public:
		static const std::map<log_level, std::string> level_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		csv_log_stream(std::ostream& target);
		virtual ~csv_log_stream() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::ostream& _target;
		std::stringstream _accumulator;
	/** @} */

	/** @name Operators */
	/** @{ */
	protected:
		virtual void accumulate() override;
		virtual bool flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id) override;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		virtual std::iostream& get_accumulator() override;
	/** @} */
	};

	class json_log_stream : public text_log_stream
	{
	/** @name Constructors */
	/** @{ */
	public:
		using text_log_stream::text_log_stream;
		virtual ~json_log_stream() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	private:
	/** @} */

	/** @name Operators */
	/** @{ */
	protected:
		virtual bool flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id) override;
	/** @} */
	};

	class xml_log_stream : public text_log_stream
	{
	/** @name Constructors */
	/** @{ */
	public:
		using text_log_stream::text_log_stream;
		virtual ~xml_log_stream() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		virtual bool flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id) override;
	/** @} */
	};

	class sys_log_stream : public text_log_stream
	{
	/** @name Classes */
	/** @{ */
	public:
		class _
		{
		friend class sys_log_stream;
		/** @name Constructors */
		/** @{ */
		private:
			_();
			~_();
		/** @} */

		/** @name Properties */
		/** @{ */
		public:
			const std::string program_name;
		/** @} */
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		sys_log_stream();
		virtual ~sys_log_stream() override = default;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
		virtual void accumulate() override;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id) override;
	/** @} */
	};
}

#endif
