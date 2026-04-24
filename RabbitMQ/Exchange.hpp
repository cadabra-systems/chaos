/**
 @file Exchange.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Exchange_hpp
#define Chaos_RabbitMQ_Exchange_hpp

#include "RabbitMQ.hpp"

#include "../BiMap.hpp"

#include <rabbitmq-c/amqp.h>
#include <memory>
#include <string_view>
#include <string>

namespace chaos { namespace rabbitmq {
	class connection;

	class exchange
	{
	friend class connection;

	/** @name Classes */
	/** @{ */
	public:
		enum class type : char
		{
			fanout,
			direct,
			topic,
			headers
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const chaos::bimap<exchange::type, std::string> type_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		exchange(const exchange&) = delete;
		~exchange();

	private:
		exchange(std::shared_ptr<state> state, amqp_channel_t channel, std::string name, exchange::type type, bool durable, bool auto_delete) noexcept;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<state> _state;
		const amqp_channel_t _channel;
		const std::string _name;
		const type _type;
		const bool _durable;
		const bool _auto_delete;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		exchange& operator=(const exchange&) = delete;
		exchange& operator=(exchange&&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		bool declare() noexcept;

	public:
		bool publish(const std::string& routing_key, std::string_view payload, std::string_view content_type = "application/octet-stream") noexcept;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_name() const noexcept;
	/** @} */
	};
} }

#endif
