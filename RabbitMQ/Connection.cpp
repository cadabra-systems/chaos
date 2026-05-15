/**
 @file Connection.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Connection.hpp"

#include "Log.hpp"
#include "Error.hpp"

namespace chaos { namespace rabbitmq {
	connection::connection(const std::string& hostname, std::uint16_t port, const std::string& vhost)
	:
		_state(std::make_shared<state>(nullptr)),
		_hostname(hostname),
		_port(port),
		_vhost(vhost)
	{

	}

	connection::connection(connection&& origin)
	:
		_state(std::move(origin._state)),
		_hostname(std::move(origin._hostname)),
		_port(origin._port),
		_vhost(std::move(origin._vhost)),
		_username(std::move(origin._username)),
		_password(std::move(origin._password))
	{

	}

	connection::~connection()
	{
		amqp_connection_state_t state(_state->exchange(nullptr));
		if (state) {
			amqp_connection_close(state, AMQP_REPLY_SUCCESS);
			amqp_destroy_connection(state);
		}
	}

	bool connection::connect(const std::string& username, const std::string& password) noexcept
	{
		if (_state->load()) {
			return true;
		}
		std::unique_ptr<amqp_connection_state_t_, decltype(&amqp_destroy_connection)> state(amqp_new_connection(), &amqp_destroy_connection);
		amqp_socket_t* socket(amqp_tcp_socket_new(state.get()));
		if (!socket) {
			chaos::log_register<rabbitmq::log>::error("connection::connect(username, password) > Failed to create TCP socket");
			return false;
		} else if (amqp_socket_open(socket, _hostname.c_str(), _port) != AMQP_STATUS_OK) {
			chaos::log_register<rabbitmq::log>::error("connection::connect(username, password) > Failed to open socket to ", _hostname, ":", _port);
			return false;
		}
		const amqp_rpc_reply_t login_reply
		(
			amqp_login
			(
				state.get(),
				_vhost.c_str(),
				0,
				AMQP_DEFAULT_FRAME_SIZE,
				0,
				AMQP_SASL_METHOD_PLAIN,
				username.c_str(),
				password.c_str()
			)
		);
		if (!error::none(login_reply)) {
			chaos::log_register<rabbitmq::log>::error("connection::connect(username, password) > Login error: ", error::message(login_reply));
			return false;
		}
		_username = username;
		_password = password;
		_state->store(state.release());
		return true;
	}

	bool connection::disconnect() noexcept
	{
		amqp_connection_state_t backup(_state->exchange(nullptr));
		if (!backup) {
			return false;
		}
		amqp_connection_close(backup, AMQP_REPLY_SUCCESS);
		amqp_destroy_connection(backup);
		return true;
	}

	bool connection::reconnect() noexcept
	{
		return disconnect() && connect(_username, _password);
	}

	bool connection::alive(bool revive) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (state) {
			amqp_frame_t frame{};
			frame.frame_type = AMQP_FRAME_HEARTBEAT;
			frame.channel = 0;
			if (amqp_send_frame(state, &frame) == AMQP_STATUS_OK) {
				return true;
			}
		}
		return revive && reconnect();
	}

	bool connection::declare(exchange::type type, const std::string& name, bool durable, bool auto_delete) noexcept
	{
		amqp_connection_state_t state(_state->load());
		if (!state) {
			return false;
		} else if (
					!amqp_exchange_declare
					(
						state,
						connection::default_channel,
						amqp_cstring_bytes(name.c_str()),
						amqp_cstring_bytes(exchange::type_map.at(type).c_str()),
						0,
						durable ? 1 : 0,
						auto_delete ? 1 : 0,
						0,
						amqp_empty_table
					)
		) {
			chaos::log_register<rabbitmq::log>::error("subscriber::declare(type, name, durable, auto_delete) > Error: ", error::message(state));
			return false;
		}
		return true;
	}

	int connection::get_file_descriptor() const noexcept
	{
		amqp_connection_state_t state(_state->load());
		return state ? amqp_get_sockfd(state) : -1;
	}

	bool connection::is_active() const noexcept
	{
		return _state && _state->load() != nullptr;
	}
} }
