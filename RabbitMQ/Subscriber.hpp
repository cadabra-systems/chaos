/**
 @file Subscriber.hpp
 @date 2026-04-28
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Subscriber_hpp
#define Chaos_RabbitMQ_Subscriber_hpp

#include "Connection.hpp"
#include "Message.hpp"

#include <rabbitmq-c/amqp.h>

#include <set>
#include <string>
#include <utility>

namespace chaos { namespace rabbitmq {
	class subscriber : public connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		subscriber(const std::string& hostname, std::uint16_t port, const std::string& vhost = "/", bool auto_ack = false);
		subscriber(subscriber&&);
		subscriber(const subscriber&) = delete;
		virtual ~subscriber() override;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const bool _auto_ack;
		amqp_bytes_t _tag;
		std::string _queue_name;
		std::set<std::pair<std::string, std::string>> _binding_set;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		subscriber& operator=(subscriber&&) = delete;
		subscriber& operator=(const subscriber&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool connect(const std::string& username, const std::string& password) noexcept override;
		virtual bool disconnect() noexcept override;

		bool bind(const std::string& exchange_name, const std::string& routing_key) noexcept;
		bool unbind(const std::string& exchange_name, const std::string& routing_key) noexcept;
		bool ack(const message& message) noexcept;
		bool flow(bool onoff) noexcept;
		message poll(int timeout = 0) noexcept;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_tagged() const noexcept;
	/** @} */
	};
} }

#endif
