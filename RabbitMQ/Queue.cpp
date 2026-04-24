/**
 @file Queue.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Queue.hpp"
#include "Error.hpp"
#include "Log.hpp"

namespace chaos { namespace rabbitmq {
	queue::queue(std::shared_ptr<state> state, amqp_channel_t channel, std::string name, bool durable, bool exclusive, bool auto_delete) noexcept
	:
		_state(std::move(state)),
		_channel(channel),
		_name(std::move(name)),
		_durable(durable),
		_exclusive(exclusive),
		_auto_delete(auto_delete),
		_subscribed(false),
		_auto_ack(false)
	{

	}

	queue::~queue()
	{
		amqp_connection_state_t state(_state->load());
		if (state) {
			amqp_channel_close(state, _channel, AMQP_REPLY_SUCCESS);
		}
	}

	bool queue::declare() noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		} else if (!amqp_channel_open(state, _channel)) {
			chaos::log_register<rabbitmq::log>::error("queue::declare > Open error");
			return false;
		} else if (
					!amqp_queue_declare
					(
						state,
						_channel,
						amqp_cstring_bytes(_name.c_str()),
						0,
						_durable ? 1 : 0,
						_exclusive ? 1 : 0,
						_auto_delete ? 1 : 0,
						amqp_empty_table
					)
		) {
			chaos::log_register<rabbitmq::log>::error("queue::declare > Error: ", error::message(state));
			return false;
		}
		return true;
	}

	bool queue::rebind() noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		}
		for (const auto& [exchange_name, routing_key] : _binding_set) {
			if (
				!amqp_queue_bind
				(
					state,
					_channel,
					amqp_cstring_bytes(_name.c_str()),
					amqp_cstring_bytes(exchange_name.c_str()),
					amqp_cstring_bytes(routing_key.c_str()),
					amqp_empty_table
				)
			) {
				chaos::log_register<rabbitmq::log>::error("queue::rebind > Error: " + error::message(state));
				return false;
			}
		}
		return true;
	}

	bool queue::resubscribe() noexcept
	{
		if (!_subscribed) {
			return true;
		}
		_subscribed = false;
		return subscribe(_auto_ack);
	}

	bool queue::bind(const std::string& exchange_name, const std::string& routing_key) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		} else if (
					!amqp_queue_bind
					(
						state,
						_channel,
						amqp_cstring_bytes(_name.c_str()),
						amqp_cstring_bytes(exchange_name.c_str()),
						amqp_cstring_bytes(routing_key.c_str()),
						amqp_empty_table
					)
		) {
			chaos::log_register<rabbitmq::log>::error("queue::bind > Error: " + error::message(state));
			return false;
		}
		_binding_set.emplace(exchange_name, routing_key);
		return true;
	}

	bool queue::subscribe(bool auto_ack) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		} else if (
					!amqp_basic_consume
					(
						state,
						_channel,
						amqp_cstring_bytes(_name.c_str()),
						amqp_empty_bytes,
						0,
						auto_ack ? 1 : 0,
						0,
						amqp_empty_table
					)
		) {
			chaos::log_register<rabbitmq::log>::error("queue::subscribe > Error: ", error::message(state));
			return false;
		}
		_subscribed = true;
		_auto_ack = auto_ack;
		return true;
	}

	bool queue::unsubscribe() noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state || !_subscribed) {
			return false;
		} else if (!amqp_channel_flow(state, _channel, 0)) {
			chaos::log_register<rabbitmq::log>::error("queue::unsubscribe > Error: ", error::message(state));
			return false;
		}
		_subscribed = false;
		return true;
	}

	const std::string& queue::get_name() const noexcept
	{
		return _name;
	}
} }
