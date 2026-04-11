/**
 @file Log.cpp
 @date 24.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "Log.hpp"

namespace chaos {
	log::log()
	:
		_prefix("Chaos"),
		_method(&log::relay)
	{

	}

	log::log(const std::string& prefix)
	:
		_prefix(prefix),
		_method(&log::relay)
	{

	}

	log::~log()
	{
		while (flush() > 0) {
			/// ...
		}
	}

	bool log::map(std::ostream& stream, log_level level, log_format format)
	{
		multimap::const_iterator s(_map.cend());
		switch (format) {
			case log_format::text:
				s = _map.emplace(level, std::make_unique<text_log_stream>(stream));
				break;

			case log_format::message:
				s = _map.emplace(level, std::make_unique<message_log_stream>(stream));
				break;

			case log_format::csv:
				s = _map.emplace(level, std::make_unique<csv_log_stream>(stream));
				break;

			case log_format::json:
				s = _map.emplace(level, std::make_unique<json_log_stream>(stream));
				break;

			case log_format::xml:
				s = _map.emplace(level, std::make_unique<xml_log_stream>(stream));
				break;

			default:
				return false;
		}
		return (_map.cend() == s) ? false : s->second->probe(_prefix);
	}

	bool log::map(std::unique_ptr<log_stream>&& stream, log_level level)
	{
		return stream ? _map.emplace(level, std::move(stream))->second->probe(_prefix) : false;
	}

	void log::unmap()
	{
		_map.clear();
	}

	void log::add(log_level&& level, std::string&& message)
	{
		(this->*_method)(std::move(level), std::move(message));
	}

	void log::relay(log_level&& level, std::string&& message)
	{
		multimap::iterator s(_map.lower_bound(level));
		while (_map.end() != s) {
			s++->second->accumulate(_prefix, level, std::this_thread::get_id(), time{true}, message);
		}
	}

	void log::skip(log_level&& level, std::string&& message)
	{
		/// @note Yeah, we mean it.
	}

	void log::stash(log_level&& level, std::string&& message)
	{
		_queue.enqueue(log::record{std::move(level), std::this_thread::get_id(), time{true}, message});
	}

	std::size_t log::flush()
	{
		std::array<log::record, 100> record_array;
		const std::size_t count(_queue.try_dequeue_bulk(record_array.begin(), record_array.size()));
		for (std::size_t r = 0; r < count; ++r) {
			const log::record& record(record_array.at(r));
			multimap::iterator s(_map.lower_bound(record.level));
			while (_map.end() != s) {
				s++->second->accumulate(_prefix, record.level, record.thread_id, record.time, record.message);
			}
		}
		return count;
	}

	const std::string& log::get_prefix() const
	{
		return _prefix;
	}

	bool log::set_mode(mode value)
	{
		switch (value) {
			case mode::drain:
				_method = &log::skip;
				break;

			case mode::stream:
				_method = &log::relay;
				break;

			case mode::queue:
				_method = &log::stash;
				break;
		}
		return true;
	}
}
