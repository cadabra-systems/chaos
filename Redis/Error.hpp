/**
 * @file Error.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Error_hpp
#define Chaos_Redis_Error_hpp

#include "Redis.hpp"

#include <string>

namespace chaos { namespace redis {
	class error
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class code : std::int8_t
		{
			reply = REDIS_ERR,
			io = REDIS_ERR_IO, /// < There was an I/O error while creating the connection, trying to write to the socket or read from the socket
			eof = REDIS_ERR_EOF, /// < The server closed the connection which resulted in an empty read
			parser = REDIS_ERR_PROTOCOL, /// < There was an error while parsing the protocol
			memory = REDIS_ERR_OOM,
			timeout = REDIS_ERR_TIMEOUT,
			other = REDIS_ERR_OTHER /// < Currently, it is only used when a specified hostname to connect to cannot be resolved
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		error();
		error(error::code code, const std::string& message);

		error(redisContext* context);
		error(const context_ptr& reply);

		error(redisReply* reply);
		error(const reply_ptr& reply);

		error(error&& origin);
		error(const error& origin);

		~error() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		code _code;
		std::string _message;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		error& operator=(error&&);
		error& operator=(error const&);
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		error::code get_code() const;
		std::string get_message() const;
	/** @} */
	};

} }

#endif
