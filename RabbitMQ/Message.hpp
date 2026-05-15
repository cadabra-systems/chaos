/**
 @file Message.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Message_hpp
#define Chaos_RabbitMQ_Message_hpp

#include "RabbitMQ.hpp"

#include <rabbitmq-c/amqp.h>

#include <cstdint>
#include <string_view>

namespace chaos { namespace rabbitmq {
	class message
	{
	/** @name Constructors */
	/** @{ */
	public:
		message() noexcept;
		message(message&&) noexcept;
		message(const message&) = delete;
		explicit message(amqp_envelope_t&& envelope) noexcept;
		~message();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		amqp_envelope_t _envelope;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		message& operator=(message&&) noexcept;
		message& operator=(const message&) = delete;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		std::string_view get_exchange_name() const noexcept;
		std::string_view get_routing_key() const noexcept;
		std::string_view get_payload_data() const noexcept;
		std::string_view get_content_type() const noexcept;
		amqp_channel_t get_channel() const noexcept;
		std::uint64_t get_delivery_tag() const noexcept;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_empty() const noexcept;
		bool is_redelivered() const noexcept;
	/** @} */
	};
} }

#endif
