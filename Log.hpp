/**
 @file Log.hpp
 @date 24.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Log_hpp
#define Chaos_Log_hpp

#include "LogStream.hpp"
#include "LogRegister.hpp"
#include "BlockingQueue.hpp"
#include "Time.hpp"

#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <utility>

namespace chaos {
	class log
	{
	/** @name Aliases */
	/** @{ */
	public:
		using multimap = std::multimap<log_level, std::unique_ptr<log_stream>>;
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
		struct record
		{
			log_level level;
			std::thread::id thread_id;
			chaos::time time;
			std::string message;
		};
		enum class mode : char
		{
			drain = 'D',
			queue = 'Q',
			stream = 'S'
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		log();
		log(const std::string& prefix);
		virtual ~log();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _prefix;

		multimap _map;

		blocking_atomic_queue<log::record> _queue;
		void (log::*_method)
		(
			log_level&&,
			std::string&&
		);
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool map(std::ostream& stream, log_level level, log_format format);
		bool map(std::unique_ptr<log_stream>&& stream, log_level level);
		void unmap();

		void add(log_level&& level, std::string&& message);
		void relay(log_level&& level, std::string&& message);
		void skip(log_level&& level, std::string&& message);
		void stash(log_level&& level, std::string&& message);

		std::size_t flush();
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_prefix() const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		bool set_mode(mode value);
	/** @} */
	};
}

#endif
