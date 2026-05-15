/**
 @file Message.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Message.hpp"

#include <cstring>

namespace chaos { namespace rabbitmq {
	message::message() noexcept
	:
		_envelope{}
	{

	}

	message::message(message&& origin) noexcept
	:
		_envelope(origin._envelope)
	{
		std::memset(&origin._envelope, 0, sizeof(amqp_envelope_t));
	}

	message::message(amqp_envelope_t&& envelope) noexcept
	:
		_envelope(envelope)
	{
		std::memset(&envelope, 0, sizeof(amqp_envelope_t));
	}

	message::~message()
	{
		amqp_destroy_envelope(&_envelope);
	}

	message& message::operator=(message&& origin) noexcept
	{
		amqp_destroy_envelope(&_envelope);
		_envelope = origin._envelope;
		std::memset(&origin._envelope, 0, sizeof(amqp_envelope_t));
		return *this;
	}

	std::string_view message::get_exchange_name() const noexcept
	{
		return std::string_view(static_cast<const char*>(_envelope.exchange.bytes), _envelope.exchange.len);
	}

	std::string_view message::get_routing_key() const noexcept
	{
		return std::string_view(static_cast<const char*>(_envelope.routing_key.bytes), _envelope.routing_key.len);
	}

	std::string_view message::get_payload_data() const noexcept
	{
		return std::string_view(static_cast<const char*>(_envelope.message.body.bytes), _envelope.message.body.len);
	}

	std::string_view message::get_content_type() const noexcept
	{
		if (!(_envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG)) {
			return std::string_view{};
		}
		return std::string_view(static_cast<const char*>(_envelope.message.properties.content_type.bytes), _envelope.message.properties.content_type.len);
	}

	amqp_channel_t message::get_channel() const noexcept
	{
		return _envelope.channel;
	}

	std::uint64_t message::get_delivery_tag() const noexcept
	{
		return _envelope.delivery_tag;
	}

	bool message::is_empty() const noexcept
	{
		return _envelope.delivery_tag == 0;
	}

	bool message::is_redelivered() const noexcept
	{
		return _envelope.redelivered != 0;
	}
} }
