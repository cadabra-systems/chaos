/**
 @file Log.hpp
 @date 24.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Log_hpp
#define Chaos_Log_hpp

#include "LogRegister.hpp"
#include "LogStream.hpp"
#include "WorkerThread.hpp"

#include <map>
#include <memory>
#include <future>
#include <utility>
#include <iostream>
#include <functional>

namespace chaos {
	class log
	{
	/** @name Constants */
	/** @{ */
	public:
		using map = std::multimap<log_level, std::unique_ptr<log_stream>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		log() : _prefix("Chaos") {}
		log(const std::string& prefix) : _prefix(prefix) {}
		virtual ~log() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _prefix;
		map _map;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual void clear()
		{
			_map.clear();
		}
		
		virtual bool add(std::ostream& stream, log_level level, log_format format)
		{
			map::const_iterator s(_map.cend());
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

		virtual bool add(std::unique_ptr<log_stream>&& stream, log_level level)
		{
			return stream ? _map.emplace(level, std::move(stream))->second->probe(_prefix) : false;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_prefix() const
		{
			return _prefix;
		}
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Templates */
	/** @{ */
	public:
		template<class... Args>
		void operator()(const log_level& level, const std::thread::id& thread_id, const Args&... args)
		{
			map::iterator s(_map.lower_bound(level));
			while (_map.end() != s) {
				s++->second->accumulate(_prefix, level, thread_id, args...);
			}
		}
	/** @} */
	};	
}

#endif
