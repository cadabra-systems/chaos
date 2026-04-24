/**
 @file Error.cpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Error.hpp"

#include "Connection.hpp"
#include "Log.hpp"

namespace chaos { namespace rabbitmq {
	bool error::none(const amqp_rpc_reply_t& reply) noexcept
	{
		return (AMQP_RESPONSE_NORMAL == reply.reply_type);
	}

	std::uint64_t error::code(const amqp_rpc_reply_t& reply) noexcept
	{
		switch (reply.reply_type) {
			case AMQP_RESPONSE_NORMAL:
				return static_cast<std::uint64_t>(error::type::none);

			case AMQP_RESPONSE_NONE:
				return static_cast<std::uint64_t>(error::type::response);

			case AMQP_RESPONSE_LIBRARY_EXCEPTION:
				return static_cast<std::uint64_t>(error::type::client);

			case AMQP_RESPONSE_SERVER_EXCEPTION:
				if (AMQP_CHANNEL_CLOSE_METHOD == reply.reply.id) {
					const amqp_channel_close_t* e(reinterpret_cast<amqp_channel_close_t*>(reply.reply.decoded));
					return (static_cast<std::uint64_t>(error::type::channel) | e->reply_code);
				} else if (AMQP_CONNECTION_CLOSE_METHOD == reply.reply.id) {
					const amqp_connection_close_t* e(reinterpret_cast<amqp_connection_close_t*>(reply.reply.decoded));
					return (static_cast<std::uint64_t>(error::type::connection) | e->reply_code);
				}
				return (static_cast<std::uint64_t>(error::type::server) | reply.reply.id);
		}
		return static_cast<std::uint64_t>(error::type::unknown);
	}

	std::string error::message(const amqp_rpc_reply_t& reply) noexcept
	{
		switch (reply.reply_type) {
			case AMQP_RESPONSE_NONE:
				return "Missing RPC reply";

			case AMQP_RESPONSE_LIBRARY_EXCEPTION:
				return amqp_error_string2(reply.library_error);

			case AMQP_RESPONSE_SERVER_EXCEPTION:
				if (AMQP_CHANNEL_CLOSE_METHOD == reply.reply.id) {
					const amqp_channel_close_t* e(reinterpret_cast<amqp_channel_close_t*>(reply.reply.decoded));
					return {static_cast<const char*>(e->reply_text.bytes), e->reply_text.len};
				} else if (AMQP_CONNECTION_CLOSE_METHOD == reply.reply.id) {
					const amqp_connection_close_t* e(reinterpret_cast<amqp_connection_close_t*>(reply.reply.decoded));
					return {static_cast<const char*>(e->reply_text.bytes), e->reply_text.len};
				}
				return "Unknown server exception";

			default:
				return "Unknown error";
		}
		return {};
	}

	error::error(const rabbitmq::connection& connection) noexcept
	:
		error(amqp_get_rpc_reply(connection._state->load()))
	{

	}

	error::error(const amqp_rpc_reply_t& reply) noexcept
	:
		_code(error::code(reply)),
		_message(error::message(reply))
	{

	}

	error::error(error&& origin) noexcept
	:
		_code(std::move(origin._code)),
		_message(std::move(origin._message))
	{
		origin._code = 0;
		origin._message.clear();
	}

	error::error(const error& origin) noexcept
	:
		_code(origin._code),
		_message(origin._message)
	{

	}

	void error::log() const
	{
		switch (get_type()) {
			case type::none:
				return ;

			case type::response:
				return chaos::log_register<rabbitmq::log>::error("Response error(", _code, "): ", _message);

			case type::client:
				return chaos::log_register<rabbitmq::log>::error("Client error(", _code, "): ", _message);

			case type::server:
				return chaos::log_register<rabbitmq::log>::error("Server error(", _code, "): ", _message);

			case type::channel:
				return chaos::log_register<rabbitmq::log>::error("Channel error(", _code, "): ", _message);

			case type::connection:
				return chaos::log_register<rabbitmq::log>::error("Connection error(", _code, "): ", _message);

			default:
				return chaos::log_register<rabbitmq::log>::error("Unknown error(", _code, "): ", _message);
		}
	}

	error::type error::get_type() const noexcept
	{
		return static_cast<error::type>(0xFFFFFFFF00000000 & _code);
	}

	std::uint32_t error::get_code() const noexcept
	{
		return static_cast<std::uint32_t>(_code & 0x00000000FFFFFFFF);
	}

	const std::string& error::get_message() const noexcept
	{
		return _message;
	}
} }
