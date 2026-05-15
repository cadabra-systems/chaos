/**
 @file Publisher.cpp
 @date 2026-04-28
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Publisher.hpp"
#include "Error.hpp"
#include "Log.hpp"

namespace chaos { namespace rabbitmq {
	publisher::publisher(const std::string& hostname, std::uint16_t port, const std::string& vhost)
	:
		connection(hostname, port, vhost)
	{

	}

	publisher::publisher(publisher&& origin)
	:
		connection(std::move(origin))
	{

	}

	publisher::~publisher()
	{
		amqp_connection_state_t state(_state->load());
		if (state && connection::default_channel > 0) {
			amqp_channel_close(state, connection::default_channel, AMQP_REPLY_SUCCESS);
		}
	}

	bool publisher::connect(const std::string& username, const std::string& password) noexcept
	{
		if (!connection::connect(username, password)) {
			return false;
		}

		amqp_connection_state_t state(_state->load());
		if (!amqp_channel_open(state, connection::default_channel)) {
			chaos::log_register<rabbitmq::log>::error("publisher::connect > Failed to open channel");
			connection::disconnect();
			return false;
		}
		return true;
	}

	bool publisher::disconnect() noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (state) {
			amqp_channel_close(state, connection::default_channel, AMQP_REPLY_SUCCESS);
		}
		return connection::disconnect();
	}

	bool publisher::push(const std::string& exchange_name, const std::string& routing_key, std::string_view payload, std::string_view content_type, const std::map<std::string, std::string>& header_map) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		}

		amqp_basic_properties_t property;
		property._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
		property.content_type = amqp_bytes_t{content_type.size(), const_cast<char*>(content_type.data())};
		property.delivery_mode = 2;

		amqp_table_t header_table{};
		std::vector<amqp_table_entry_t> header_vector;
		if (!header_map.empty()) {
			header_vector.reserve(header_map.size());
			for (const auto& [name, value] : header_map) {
				amqp_table_entry_t entry;
				entry.key = amqp_cstring_bytes(name.c_str());
				entry.value.kind = AMQP_FIELD_KIND_UTF8;
				entry.value.value.bytes = amqp_cstring_bytes(value.c_str());
				header_vector.push_back(entry);
			}
			header_table.num_entries = static_cast<int>(header_vector.size());
			header_table.entries = header_vector.data();
			property._flags |= AMQP_BASIC_HEADERS_FLAG;
			property.headers = header_table;
		}

		const amqp_bytes_t body{payload.size(), const_cast<char*>(payload.data())};
		const int status
		(
			amqp_basic_publish
			(
				state,
				connection::default_channel,
				amqp_cstring_bytes(exchange_name.c_str()),
				amqp_cstring_bytes(routing_key.c_str()),
				0,
				0,
				&property,
				body
			)
		);
		if (status != AMQP_STATUS_OK) {
			chaos::log_register<rabbitmq::log>::error("publisher::push > Error: ", amqp_error_string2(status));
			return false;
		}
		return true;
	}
} }
