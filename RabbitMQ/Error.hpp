/**
 @file Error.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Error_hpp
#define Chaos_RabbitMQ_Error_hpp

#include "RabbitMQ.hpp"

#include <rabbitmq-c/amqp.h>
#include <string>
#include <cstdint>

namespace chaos { namespace rabbitmq {
	class connection;

	class error
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class type : std::uint64_t
		{
			none = 0x0000000000000000,
			response = 0x100000000000000,
			client = 0x200000000000000,
			server = 0x300000000000000,
			connection = 0x400000000000000,
			channel = 0x500000000000000,
			unknown = 0x600000000000000
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static bool none(const amqp_rpc_reply_t& reply) noexcept;
		static std::uint64_t code(const amqp_rpc_reply_t& reply) noexcept;
		static std::string message(const amqp_rpc_reply_t& reply) noexcept;

		static inline bool none(const amqp_connection_state_t& state) noexcept
		{
			return none(amqp_get_rpc_reply(state));
		}

		static inline std::uint64_t code(const amqp_connection_state_t& state) noexcept
		{
			return code(amqp_get_rpc_reply(state));
		}

		static inline std::string message(const amqp_connection_state_t& state) noexcept
		{
			return message(amqp_get_rpc_reply(state));
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		error(const rabbitmq::connection& connection) noexcept;
		error(const amqp_rpc_reply_t& reply) noexcept;
		error(error&& origin) noexcept;
		error(const error& origin) noexcept;
		~error() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::uint64_t _code;
		std::string _message;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void log() const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		type get_type() const noexcept;
		std::uint32_t get_code() const noexcept;
		const std::string& get_message() const noexcept;
	/** @} */
	};
} }

#endif
