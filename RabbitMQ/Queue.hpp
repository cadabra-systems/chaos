/**
 @file Queue.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Queue_hpp
#define Chaos_RabbitMQ_Queue_hpp

#include "RabbitMQ.hpp"

#include <rabbitmq-c/amqp.h>
#include <memory>
#include <set>
#include <string>
#include <utility>

namespace chaos { namespace rabbitmq {
	class connection;

	class queue
	{
	friend class connection;

	/** @name Aliases */
	/** @{ */
	public:
		using binding_set = std::set<std::pair<std::string, std::string>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		queue(const queue&) = delete;
		~queue();
	private:
		queue(std::shared_ptr<state> state, amqp_channel_t channel, std::string name, bool durable, bool exclusive, bool auto_delete) noexcept;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<state> _state;
		const amqp_channel_t _channel;
		const std::string _name;
		const bool _durable;
		const bool _exclusive;
		const bool _auto_delete;
		binding_set _binding_set;
		bool _subscribed;
		bool _auto_ack;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		queue& operator=(const queue&) = delete;
		queue& operator=(queue&&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		bool declare() noexcept;
		bool rebind() noexcept;
		bool resubscribe() noexcept;

	public:
		bool bind(const std::string& exchange_name, const std::string& routing_key) noexcept;
		bool subscribe(bool auto_ack = false) noexcept;
		bool unsubscribe() noexcept;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& get_name() const noexcept;
	/** @} */
	};
} }

#endif
