/**
 @file Exchange.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Exchange.hpp"
#include "Error.hpp"
#include "Log.hpp"

namespace chaos { namespace rabbitmq {
	const chaos::bimap<exchange::type, std::string> exchange::type_map =
	{
		{
			{exchange::type::fanout, "fanout"},
			{exchange::type::direct, "direct"},
			{exchange::type::topic, "topic"},
			{exchange::type::headers, "headers"}
		},
		exchange::type::direct
	};

	exchange::exchange(std::shared_ptr<state> state, amqp_channel_t channel, std::string name, exchange::type type, bool durable, bool auto_delete) noexcept
	:
		_state(std::move(state)),
		_channel(channel),
		_name(std::move(name)),
		_type(type),
		_durable(durable),
		_auto_delete(auto_delete)
	{

	}

	exchange::~exchange()
	{
		amqp_connection_state_t state(_state->load());
		if (state) {
			amqp_channel_close(state, _channel, AMQP_REPLY_SUCCESS);
		}
	}

	bool exchange::declare() noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		} else if (!amqp_channel_open(state, _channel)) {
			chaos::log_register<rabbitmq::log>::error("exchange::declare > Open error");
			return false;
		} else if (
					!amqp_exchange_declare
					(
						state,
						_channel,
						amqp_cstring_bytes(_name.c_str()),
						amqp_cstring_bytes(exchange::type_map.at(_type).c_str()),
						0,
						_durable ? 1 : 0,
						_auto_delete ? 1 : 0,
						0,
						amqp_empty_table
					)
		) {
			chaos::log_register<rabbitmq::log>::error("exchange::declare > Error: ", error::message(state));
			return false;
		} else if (!error::none(state)) {
			chaos::log_register<rabbitmq::log>::error("exchange::declare > Error: ", error::message(state));
			return false;
		}
		return true;
	}

	bool exchange::publish(const std::string& routing_key, std::string_view payload, std::string_view content_type) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		}

		amqp_basic_properties_t properties;
		properties._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
		properties.content_type = amqp_bytes_t{content_type.size(), const_cast<char*>(content_type.data())};
		properties.delivery_mode = 2;

		const amqp_bytes_t body{payload.size(), const_cast<char*>(payload.data())};
		const int status
		(
			amqp_basic_publish
			(
				state,
				_channel,
				amqp_cstring_bytes(_name.c_str()),
				amqp_cstring_bytes(routing_key.c_str()),
				0,
				0,
				&properties,
				body
			)
		);
		if (status != AMQP_STATUS_OK) {
			chaos::log_register<rabbitmq::log>::error("exchange::publish > Error: ", amqp_error_string2(status));
			return false;
		}
		return true;
	}

	const std::string& exchange::get_name() const noexcept
	{
		return _name;
	}
} }
