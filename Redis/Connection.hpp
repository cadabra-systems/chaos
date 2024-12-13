/**
 @file Connection.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Connection_hpp
#define Chaos_Redis_Connection_hpp

#include "Command.hpp"
#include "Redis.hpp"

#include <string>
#include <memory>

namespace chaos { namespace redis {
	class connection
	{
	/** @name Classes */
	/** @{ */
	public:
		class error
		{
		/** @name Classes */
		/** @{ */
		public:
			enum class incident : int
			{
				ok = 0,
				io = REDIS_ERR_IO, /// < There was an I/O error while creating the connection, trying to write to the socket or read from the socket. If you included errno.h in your application, you can use the global errno variable to find out what is wrong
				eof = REDIS_ERR_EOF, /// < The server closed the connection which resulted in an empty read
				parser = REDIS_ERR_PROTOCOL, /// < There was an error while parsing the protocol
				memory = REDIS_ERR_OOM,
				other = REDIS_ERR_OTHER /// < Any other error. Currently, it is only used when a specified hostname to connect to cannot be resolved.
			};
		/** @} */
			
		/** @name Constructors */
		/** @{ */
		public:
			error(const connection& connection);
			~error() = default;
		/** @} */
			
		/** @name Properties */
		/** @{ */
		private:
			const connection& _connection;
		/** @} */
			
		/** @name Operators */
		/** @{ */
		public:
			error& operator=(error const&) = delete;
		/** @} */
			
		/** @name Procedures */
		/** @{ */
		public:
		/** @} */
		
		/** @name Getters */
		/** @{ */
		public:
			std::string get_message() const;
			error::incident get_incident() const;
		/** @} */
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		connection(const std::string& host, const std::uint16_t port = 6379);
		connection(connection const&) = delete;
		virtual ~connection();
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _host;
		const std::uint16_t _port;
		
		redisContext* _handle;
		connection::error _error;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		connection& operator=(connection const&) = delete;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		bool connect();
		bool reconnect();
		bool disconnect();
		bool send(command& command) const;
		bool send(std::initializer_list<std::reference_wrapper<command>> list, bool transaction_mode = false) const;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:

	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const connection::error& get_error() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool is_connected() const;
	/** @} */
	};
} }

#endif
