/**
 @file Connection.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Connection_hpp
#define Chaos_RabbitMQ_Connection_hpp

#include "Message.hpp"
#include "Exchange.hpp"
#include "Queue.hpp"

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>

#include <cstdint>
#include <memory>
#include <set>
#include <string>

namespace chaos { namespace rabbitmq {
	class connection
	{
	friend class error;

	/** @name Constructors */
	/** @{ */
	public:
		connection(const std::string& hostname, std::uint16_t port, const std::string& vhost = "/");
		connection(connection&& origin);
		connection(const connection&) = delete;
		~connection();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<state> _state;

		const std::string _hostname;
		const std::uint16_t _port;
		const std::string _vhost;

		std::string _username;
		std::string _password;

		amqp_channel_t _channel_counter;

		std::set<std::weak_ptr<exchange>, std::owner_less<std::weak_ptr<exchange>>> _exchange_set;
		std::set<std::weak_ptr<queue>, std::owner_less<std::weak_ptr<queue>>> _queue_set;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		connection& operator=(connection&&) = delete;
		connection& operator=(const connection&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		bool restore() noexcept;

	public:
		bool connect(const std::string& username, const std::string& password) noexcept;
		bool reconnect() noexcept;
		bool disconnect() noexcept;
		bool alive() noexcept;

		message poll(int timeout_ms = 0) noexcept;
		bool ack(const message& message) noexcept;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		std::shared_ptr<exchange> make_exchange(const std::string& name, exchange::type type = exchange::type::direct, bool durable = true, bool auto_delete = false) noexcept;
		std::shared_ptr<queue> make_queue(const std::string& name, bool durable = true, bool exclusive = false, bool auto_delete = false) noexcept;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_active() const noexcept;
	/** @} */
	};
} }

#endif
