/**
 @file Connection.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Connection.hpp"
#include "Error.hpp"
#include "Log.hpp"

#include <memory>

namespace chaos { namespace rabbitmq {
	connection::connection(const std::string& hostname, std::uint16_t port, const std::string& vhost)
	:
		_state(std::make_shared<state>(nullptr)),

		_hostname(hostname),
		_port(port),
		_vhost(vhost),

		_channel_counter(0)
	{

	}

	connection::connection(connection&& origin)
	:
		_state(std::move(origin._state)),

		_hostname(std::move(origin._hostname)),
		_port(origin._port),
		_vhost(std::move(origin._vhost)),

		_username(std::move(origin._username)),
		_password(std::move(origin._password)),

		_channel_counter(origin._channel_counter),
		_exchange_set(std::move(origin._exchange_set)),
		_queue_set(std::move(origin._queue_set))
	{
		origin._channel_counter = 0;
	}

	connection::~connection()
	{
		amqp_connection_state_t state(_state->exchange(nullptr));
		if (state) {
			amqp_connection_close(state, AMQP_REPLY_SUCCESS);
			amqp_destroy_connection(state);
		}
	}

	bool connection::restore() noexcept
	{
		for (auto iterator(_exchange_set.begin()); iterator != _exchange_set.end(); ) {
			std::shared_ptr<exchange> exchange(iterator->lock());
			if (!exchange) {
				iterator = _exchange_set.erase(iterator);
				continue;
			} else if (!exchange->declare()) {
				return false;
			}
			++iterator;
		}
		for (auto iterator(_queue_set.begin()); iterator != _queue_set.end(); ) {
			std::shared_ptr<queue> queue(iterator->lock());
			if (!queue) {
				iterator = _queue_set.erase(iterator);
				continue;
			} else if (!queue->declare() || !queue->rebind() || !queue->resubscribe()) {
				return false;
			}
			++iterator;
		}
		return true;
	}

	bool connection::connect(const std::string& username, const std::string& password) noexcept
	{
		if (_state->load()) {
			return true;
		}

		std::unique_ptr<amqp_connection_state_t_, decltype(&amqp_destroy_connection)> state(amqp_new_connection(), &amqp_destroy_connection);
		amqp_socket_t* socket(amqp_tcp_socket_new(state.get()));
		if (!socket) {
			chaos::log_register<rabbitmq::log>::error("connection::connect > Failed to create TCP socket");
			return false;
		} else if (amqp_socket_open(socket, _hostname.c_str(), _port) != AMQP_STATUS_OK) {
			chaos::log_register<rabbitmq::log>::error("connection::connect > Failed to open socket to ", _hostname, ":", _port);
			return false;
		}
		amqp_rpc_reply_t login_reply
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
			chaos::log_register<rabbitmq::log>::error("connection::connect > Login error: ", error::message(login_reply));
			return false;
		}

		_username = username;
		_password = password;
		_state->store(state.release());
		return true;
	}

	bool connection::reconnect() noexcept
	{
		disconnect();
		return connect(_username, _password) && restore();
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

	bool connection::alive() noexcept
	{
		return _state->load() != nullptr || reconnect();
	}

	message connection::poll(int timeout) noexcept
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
			chaos::log_register<rabbitmq::log>::error("connection::poll > Timeout");
			return {};
		} else if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
			chaos::log_register<rabbitmq::log>::error("connection::poll > Error: ", error::message(reply));
			return {};
		}
		return message(std::move(envelope));
	}

	bool connection::ack(const message& message) noexcept
	{
		amqp_connection_state_t state(_state->load());
		return state && (amqp_basic_ack(state, message.get_channel(), message.get_delivery_tag(), 0) == AMQP_STATUS_OK);
	}

	std::shared_ptr<exchange> connection::make_exchange(const std::string& name, exchange::type type, bool durable, bool auto_delete) noexcept
	{
		if (!_state->load()) {
			return nullptr;
		}
		std::shared_ptr<exchange> retval(new exchange(_state, ++_channel_counter, name, type, durable, auto_delete));
		if (!retval->declare()) {
			return nullptr;
		}
		_exchange_set.emplace(retval);
		return retval;
	}

	std::shared_ptr<queue> connection::make_queue(const std::string& name, bool durable, bool exclusive, bool auto_delete) noexcept
	{
		if (!_state->load()) {
			return nullptr;
		}
		std::shared_ptr<queue> retval(new queue(_state, ++_channel_counter, name, durable, exclusive, auto_delete));
		if (!retval->declare()) {
			return nullptr;
		}
		_queue_set.emplace(retval);
		return retval;
	}

	bool connection::is_active() const noexcept
	{
		return _state && _state->load() != nullptr;
	}
} }
