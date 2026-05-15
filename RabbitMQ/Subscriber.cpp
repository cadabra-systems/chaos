/**
 @file Subscriber.cpp
 @date 2026-04-28
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Subscriber.hpp"
#include "Error.hpp"
#include "Log.hpp"

namespace chaos { namespace rabbitmq {
	subscriber::subscriber(const std::string& hostname, std::uint16_t port, const std::string& vhost, bool auto_ack)
	:
		connection(hostname, port, vhost),

		_auto_ack(auto_ack),
		_tag(amqp_empty_bytes)
	{

	}

	subscriber::subscriber(subscriber&& origin)
	:
		connection(std::move(origin)),

		_auto_ack(origin._auto_ack),
		_tag(origin._tag),
		_queue_name(std::move(origin._queue_name)),
		_binding_set(std::move(origin._binding_set))
	{
		origin._tag = amqp_empty_bytes;
	}

	subscriber::~subscriber()
	{
		amqp_connection_state_t state(_state->load());
		if (state) {
			if (_tag.len > 0) {
				amqp_basic_cancel(state, connection::default_channel, _tag);
				amqp_bytes_free(_tag);
			}
			amqp_channel_close(state, connection::default_channel, AMQP_REPLY_SUCCESS);
		}
	}

	bool subscriber::connect(const std::string& username, const std::string& password) noexcept
	{
		if (_tag.len > 0) {
			return true;
		} else if (!connection::connect(username, password)) {
			return false;
		}

		amqp_connection_state_t state(_state->load());
		if (!amqp_channel_open(state, connection::default_channel)) {
			chaos::log_register<rabbitmq::log>::error("subscriber::connect > Failed to open channel");
			connection::disconnect();
			return false;
		}

		amqp_queue_declare_ok_t* queue_result
		(
			amqp_queue_declare
			(
				state,
				connection::default_channel,
				amqp_empty_bytes,
				0,
				0,
				1,
				1,
				amqp_empty_table
			)
		);
		if (!queue_result) {
			chaos::log_register<rabbitmq::log>::error("subscriber::connect > Failed to declare queue: ", error::message(state));
			connection::disconnect();
			return false;
		}
		std::string queue_name(std::string(static_cast<const char*>(queue_result->queue.bytes), queue_result->queue.len));

		for (const auto& [exchange_name, routing_key] : _binding_set) {
			if (
				!amqp_queue_bind
				(
					state,
					connection::default_channel,
					amqp_cstring_bytes(queue_name.c_str()),
					amqp_cstring_bytes(exchange_name.c_str()),
					amqp_cstring_bytes(routing_key.c_str()),
					amqp_empty_table
				)
			) {
				chaos::log_register<rabbitmq::log>::error("subscriber::connect > Failed to rebind ", exchange_name, "/", routing_key, ": ", error::message(state));
				connection::disconnect();
				return false;
			}
		}

		amqp_basic_consume_ok_t* consume_result
		(
			amqp_basic_consume
			(
				state,
				connection::default_channel,
				amqp_cstring_bytes(queue_name.c_str()),
				amqp_empty_bytes,
				0,
				_auto_ack ? 1 : 0,
				0,
				amqp_empty_table
			)
		);
		if (!consume_result) {
			chaos::log_register<rabbitmq::log>::error("subscriber::connect > Failed to subscribe: ", error::message(state));
			connection::disconnect();
			return false;
		}
		_tag = amqp_bytes_malloc_dup(consume_result->consumer_tag);
		std::swap(_queue_name, queue_name);
		return true;
	}

	bool subscriber::disconnect() noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (state) {
			if (_tag.len > 0) {
				amqp_basic_cancel(state, connection::default_channel, _tag);
				amqp_bytes_free(_tag);
				_tag = amqp_empty_bytes;
			}
			amqp_channel_close(state, connection::default_channel, AMQP_REPLY_SUCCESS);
		}
		_queue_name.clear();
		return connection::disconnect();
	}


	bool subscriber::bind(const std::string& exchange_name, const std::string& routing_key) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		}
		if (
			!amqp_queue_bind
			(
				state,
				connection::default_channel,
				amqp_cstring_bytes(_queue_name.c_str()),
				amqp_cstring_bytes(exchange_name.c_str()),
				amqp_cstring_bytes(routing_key.c_str()),
				amqp_empty_table
			)
		) {
			chaos::log_register<rabbitmq::log>::error("subscriber::bind > Error: ", error::message(state));
			return false;
		}
		_binding_set.emplace(exchange_name, routing_key);
		return true;
	}

	bool subscriber::unbind(const std::string& exchange_name, const std::string& routing_key) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (state && !_queue_name.empty()) {
			if (
				!amqp_queue_unbind
				(
					state,
					connection::default_channel,
					amqp_cstring_bytes(_queue_name.c_str()),
					amqp_cstring_bytes(exchange_name.c_str()),
					amqp_cstring_bytes(routing_key.c_str()),
					amqp_empty_table
				)
			) {
				chaos::log_register<rabbitmq::log>::error("subscriber::unbind > Error: ", error::message(state));
				return false;
			}
		}
		_binding_set.erase({exchange_name, routing_key});
		return true;
	}

	bool subscriber::ack(const message& message) noexcept
	{
		amqp_connection_state_t state(_state->load());
		return state && (amqp_basic_ack(state, connection::default_channel, message.get_delivery_tag(), 0) == AMQP_STATUS_OK);
	}

	bool subscriber::flow(bool onoff) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state || _tag.len == 0) {
			return false;
		} else if (!amqp_channel_flow(state, connection::default_channel, onoff ? 1 : 0)) {
			chaos::log_register<rabbitmq::log>::error("subscriber::flow > Error: ", error::message(state));
			return false;
		}
		return true;
	}

	message subscriber::poll(int timeout) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return {};
		}
		amqp_maybe_release_buffers(state);

		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;

		amqp_envelope_t envelope{};
		const amqp_rpc_reply_t reply(amqp_consume_message(state, &envelope, timeout >= 0 ? &tv : nullptr, 0));
		if (reply.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION && reply.library_error == AMQP_STATUS_TIMEOUT) {
			return {};
		} else if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
			chaos::log_register<rabbitmq::log>::error("subscriber::poll > Error: ", error::message(reply));
			return {};
		}
		return message(std::move(envelope));
	}

	bool subscriber::is_tagged() const noexcept
	{
		return _tag.len > 0;
	}
} }
