/**
 @file Connection.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Connection_hpp
#define Chaos_RabbitMQ_Connection_hpp

#include "RabbitMQ.hpp"

#include "Exchange.hpp"

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>

#include <cstdint>
#include <memory>
#include <string>

namespace chaos { namespace rabbitmq {
	class connection
	{
	friend class error;

	/** @name Statics */
	/** @{ */
	public:
		static constexpr amqp_channel_t default_channel = 1;
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		connection(const std::string& hostname, std::uint16_t port, const std::string& vhost = "/");
		connection(connection&& origin);
		connection(const connection&) = delete;
		virtual ~connection();
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		std::shared_ptr<state> _state;
		const std::string _hostname;
		const std::uint16_t _port;
		const std::string _vhost;
		std::string _username;
		std::string _password;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		connection& operator=(connection&&) = delete;
		connection& operator=(const connection&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool connect(const std::string& username, const std::string& password) noexcept;
		virtual bool disconnect() noexcept;
		bool reconnect() noexcept;
		bool alive(bool revive) noexcept;

		bool declare(exchange::type type, const std::string& name, bool durable = false, bool auto_delete = false) noexcept;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		int get_file_descriptor() const noexcept;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_active() const noexcept;
	/** @} */
	};
} }

#endif
