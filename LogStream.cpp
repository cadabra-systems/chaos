/**
 @file LogStream.cpp
 @date 24.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "LogStream.hpp"

#include "BiMap.hpp"
#include "String.hpp"
#include "Time.hpp"
#include "NullStream.hpp"

#include <iomanip>

namespace chaos {
	const std::map<log_level, std::string> log_stream::level_map = {
		{log_level::dead, "X"},
		{log_level::fatal, "F"},
		{log_level::alert, "A"},
		{log_level::critical, "C"},
		{log_level::error, "E"},
		{log_level::warning, "W"},
		{log_level::checkpoint, "P"},
		{log_level::notice, "N"},
		{log_level::debug, "D"},
		{log_level::none, "_"}
	};	
	const std::hash<std::thread::id> log_stream::thread_hasher;

	log_stream::log_stream()
	:
		_fail_counter(0)
	{

	}

	void log_stream::accumulate()
	{

	}

	bool log_stream::probe(const std::string& prefix)
	{
		get_accumulator() << prefix << "(" << this << ") is probing log stream...";
		return flush("Log", log_level::critical, std::this_thread::get_id());
	}

	text_log_stream::text_log_stream(std::ostream& target)
	:
		log_stream(),

		_target(target)
	{

	}

	void text_log_stream::accumulate()
	{
		if (_target.fail()) {
			_target.clear();
		}
	}

	bool text_log_stream::flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id)
	{
		_target.clear();
		_accumulator << std::flush;
		_target << _accumulator.rdbuf() << std::endl;
		_accumulator.str({});

		return !_target.bad();
	}

	std::iostream& text_log_stream::get_accumulator()
	{
		return _accumulator;
	}

	message_log_stream::message_log_stream(std::ostream& target)
	:
		log_stream(),

		_target(target)
	{

	}

	void message_log_stream::accumulate()
	{
		if (_target.fail()) {
			_target.clear();
		}
	}

	bool message_log_stream::flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id)
	{
		_target.clear();
		_accumulator << std::flush;
		_target << "[" << log_stream::level_map.at(level)
				<< "][" << static_cast<std::string>(time(true))
				<< "][" << log_stream::thread_hasher(std::this_thread::get_id())
				<< "\\" << log_stream::thread_hasher(thread_id)
				<< "][" << prefix << "] " << _accumulator.rdbuf()
		<< std::endl;
		_accumulator.str({});

		return !_target.bad();
	}

	std::iostream& message_log_stream::get_accumulator()
	{
		return _accumulator;
	}

	csv_log_stream::csv_log_stream(std::ostream& target)
	:
		log_stream(),

		_target(target)
	{

	}

	void csv_log_stream::accumulate()
	{
		if (_target.fail()) {
			_target.clear();
		}
	}

	bool csv_log_stream::flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id)
	{
		_target.clear();
		_accumulator << std::flush;
		_target << "\"" << log_stream::level_map.at(level)
				<< "\";\"" << static_cast<std::string>(time(true))
				<< "\";\"" << log_stream::thread_hasher(std::this_thread::get_id())
				<< "\";\"" << log_stream::thread_hasher(thread_id)
				<< "\";\"" << prefix << "\";\"" << _accumulator.rdbuf() << "\""
		<< std::endl;
		_accumulator.str({});

		return !_target.bad();
	}

	std::iostream& csv_log_stream::get_accumulator()
	{
		return _accumulator;
	}

	bool json_log_stream::flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id)
	{
		_target.clear();
		_accumulator << std::flush;
		_target << flex {
							{"datetime", static_cast<std::string>(time(true))},
							{"process", std::to_string(log_stream::thread_hasher(std::this_thread::get_id()))},
							{"thread", std::to_string(log_stream::thread_hasher(thread_id))},
							{"level", log_stream::level_map.at(level)},
							{"message", string::replaced_all(_accumulator.str(), "\n", "\\n")},
							{"scope", prefix}
		}.dump() << std::endl;
		_accumulator.str({});

		return !_target.bad();
	}

	bool xml_log_stream::flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id)
	{
		_target.clear();
		/// @todo
		return true;
	}

	sys_log_stream::_::_()
	:
		program_name(program_name::get())
	{
		::openlog(program_name.data(), LOG_PID/* | LOG_PERROR*/, LOG_DAEMON);
	}

	sys_log_stream::_::~_()
	{
		::closelog();
	}

	sys_log_stream::sys_log_stream(const std::string& prefix)
	:
		text_log_stream(cnull),

		_prefix(prefix)
	{
		static const sys_log_stream::_ l;
	}

	void sys_log_stream::accumulate()
	{

	}

	bool sys_log_stream::flush(const std::string& prefix, const log_level& level, const std::thread::id& thread_id)
	{
		static const bimap<log_level, int> level_map
		{
			{
				{log_level::fatal, LOG_EMERG},
				{log_level::critical, LOG_CRIT},
				{log_level::error, LOG_ERR},
				{log_level::alert, LOG_ALERT},
				{log_level::warning, LOG_WARNING},
				{log_level::checkpoint, LOG_INFO},
				{log_level::notice, LOG_NOTICE},
				{log_level::debug, LOG_DEBUG},
				{log_level::none, LOG_DEBUG + 1}
			},
			log_level::none
		};

		_accumulator << std::flush;
		::syslog(level_map.at(level, true), "%s", _accumulator.str().c_str());
		_accumulator.str({});

		return true;
	}
}
