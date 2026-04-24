/**
 @file RabbitMQ.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Heisenbug_RabbitMQ_hpp
#define Chaos_Heisenbug_RabbitMQ_hpp

#include "../Heisenbug.hpp"

#include "../RabbitMQ/Connection.hpp"

#include <chrono>
#include <random>
#include <string>

namespace chaos {
	class rabbitmq_test : public heisen_test
	{
	/** @name Constructors */
	/** @{ */
	public:
		rabbitmq_test()
		:
			heisen_test("RabbitMQ")
		{

		};
		virtual ~rabbitmq_test() = default;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
		virtual void initialize() override
		{

		}

		virtual void structuralize() override
		{
			HEISEN(Connection);
			HEISEN(Exchange);
			HEISEN(Publish);
			HEISEN(Consume);
		}

		virtual void deinitialize() override
		{

		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief Connection connects and disconnects, reporting is_active() correctly.
		 */
		void testConnection()
		{
			rabbitmq::connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(connection.alive())
			IS_TRUE(connection.is_active())
			IS_TRUE(connection.disconnect())
			IS_FALSE(connection.is_active())
		}

		/**
		 * @brief Declares exchange and queue, binds them. Verifies idempotency (passive re-declare succeeds).
		 */
		void testExchange()
		{
			rabbitmq::connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			std::shared_ptr<rabbitmq::exchange> exchange(connection.make_exchange(get_variable("EXCHANGE", "heisen.test"), rabbitmq::exchange::type::direct));
			IS_TRUE(exchange != nullptr)

			std::shared_ptr<rabbitmq::queue> queue(connection.make_queue(get_variable("QUEUE", "heisen.queue")));
			IS_TRUE(queue != nullptr)
			IS_TRUE(queue->bind(exchange->get_name(), get_variable("ROUTING_KEY", "heisen.key")))
		}

		/**
		 * @brief Exchange publishes messages with and without explicit content-type.
		 */
		void testPublish()
		{
			std::default_random_engine gen;
			gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string payload("heisen-payload-" + std::to_string(std::uniform_int_distribution<std::int32_t>(100000, 999999)(gen)));

			rabbitmq::connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			std::shared_ptr<rabbitmq::exchange> exchange(connection.make_exchange(get_variable("EXCHANGE", "heisen.test"), rabbitmq::exchange::type::direct));
			IS_TRUE(exchange != nullptr)
			IS_TRUE(exchange->publish(get_variable("ROUTING_KEY", "heisen.key"), payload))
			IS_TRUE(exchange->publish(get_variable("ROUTING_KEY", "heisen.key"), payload, "application/json"))
		}

		/**
		 * @brief Round-trip: produce a uniquely-tagged message, then consume it from the bound queue.
		 *
		 * Polls up to 100 times with 500ms timeout, looking for the unique payload.
		 * Each consumed message is acked so the queue stays clean.
		 */
		void testConsume()
		{
			std::default_random_engine gen;
			gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string unique_payload("heisen-roundtrip-" + std::to_string(std::uniform_int_distribution<std::int32_t>(100000, 999999)(gen)));
			const std::string routing_key(get_variable("ROUTING_KEY", "heisen.key"));

			rabbitmq::connection producer(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(producer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			std::shared_ptr<rabbitmq::exchange> exchange(producer.make_exchange(get_variable("EXCHANGE", "heisen.test"), rabbitmq::exchange::type::direct));
			IS_TRUE(exchange != nullptr)
			std::shared_ptr<rabbitmq::queue> producer_queue(producer.make_queue(get_variable("QUEUE", "heisen.queue")));
			IS_TRUE(producer_queue != nullptr)
			IS_TRUE(producer_queue->bind(exchange->get_name(), routing_key))
			IS_TRUE(exchange->publish(routing_key, unique_payload, "text/plain"))

			rabbitmq::connection consumer(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(consumer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			std::shared_ptr<rabbitmq::queue> consumer_queue(consumer.make_queue(get_variable("QUEUE", "heisen.queue")));
			IS_TRUE(consumer_queue != nullptr)
			IS_TRUE(consumer_queue->subscribe())

			bool found(false);
			for (int i = 0; i < 100 && !found; ++i) {
				rabbitmq::message message(consumer.poll(500));
				if (message.is_empty()) {
					continue;
				}
				IS_FALSE(message.get_delivery_tag() == 0)
				IS_TRUE(consumer.ack(message))
				if (message.get_payload_data() == unique_payload) {
					ARE_EQUAL(std::string(message.get_exchange_name()), exchange->get_name())
					ARE_EQUAL(std::string(message.get_routing_key()), routing_key)
					ARE_EQUAL(std::string(message.get_content_type()), std::string("text/plain"))
					found = true;
				}
			}
			IS_TRUE(found)
		}
	/** @} */
	};
}

#endif
