/**
 @file RabbitMQ.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Heisenbug_RabbitMQ_hpp
#define Chaos_Heisenbug_RabbitMQ_hpp

#include "../Heisenbug.hpp"

#include "../RabbitMQ/Publisher.hpp"
#include "../RabbitMQ/Subscriber.hpp"

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
			HEISEN(Publish);
			HEISEN(Subscribe);
			HEISEN(RoundTrip);
		}

		virtual void deinitialize() override
		{

		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief Publisher and subscriber each connect and disconnect cleanly.
		 */
		void testConnection()
		{
			rabbitmq::publisher publisher(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(publisher.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(publisher.is_active())
			IS_TRUE(publisher.disconnect())
			IS_FALSE(publisher.is_active())

			rabbitmq::subscriber subscriber(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(subscriber.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(subscriber.is_tagged())
			IS_TRUE(subscriber.disconnect())
			IS_FALSE(subscriber.is_tagged())
		}

		/**
		 * @brief Publisher pushes messages with and without an explicit content-type.
		 */
		void testPublish()
		{
			std::default_random_engine generator;
			generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string payload("heisen-payload-" + std::to_string(std::uniform_int_distribution<std::int32_t>(100000, 999999)(generator)));

			rabbitmq::publisher publisher(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(publisher.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(publisher.push(get_variable("EXCHANGE", "heisen.test"), get_variable("ROUTING_KEY", "heisen.key"), payload))
			IS_TRUE(publisher.push(get_variable("EXCHANGE", "heisen.test"), get_variable("ROUTING_KEY", "heisen.key"), payload, "application/json"))
		}

		/**
		 * @brief Subscriber declares exchange, binds, and polls with no available message.
		 */
		void testSubscribe()
		{
			rabbitmq::subscriber subscriber(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(subscriber.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(subscriber.declare(rabbitmq::exchange::type::direct, get_variable("EXCHANGE", "heisen.test")))
			IS_TRUE(subscriber.bind(get_variable("EXCHANGE", "heisen.test"), get_variable("ROUTING_KEY", "heisen.key")))
			IS_TRUE(subscriber.is_tagged())

			rabbitmq::message message(subscriber.poll(100));
			IS_TRUE(message.is_empty())
		}

		/**
		 * @brief Round-trip: subscriber binds first, publisher pushes, subscriber polls and receives.
		 *
		 * Subscriber must be bound before the publisher sends so the exclusive auto-delete
		 * queue exists at routing time. Polls up to 100 times with 500 ms timeout each.
		 */
		void testRoundTrip()
		{
			std::default_random_engine generator;
			generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string unique_payload("heisen-roundtrip-" + std::to_string(std::uniform_int_distribution<std::int32_t>(100000, 999999)(generator)));
			const std::string exchange_name(get_variable("EXCHANGE", "heisen.test"));
			const std::string routing_key(get_variable("ROUTING_KEY", "heisen.key"));

			rabbitmq::subscriber subscriber(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(subscriber.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(subscriber.declare(rabbitmq::exchange::type::direct, exchange_name))
			IS_TRUE(subscriber.bind(exchange_name, routing_key))

			rabbitmq::publisher publisher(get_variable("HOST", "localhost"), get_variable("PORT", 5672), get_variable("VHOST", "/heisenbug"));
			IS_TRUE(publisher.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(publisher.push(exchange_name, routing_key, unique_payload, "text/plain"))

			bool found(false);
			for (int i = 0; i < 100 && !found; ++i) {
				rabbitmq::message message(subscriber.poll(500));
				if (message.is_empty()) {
					continue;
				}
				IS_FALSE(message.get_delivery_tag() == 0)
				IS_TRUE(subscriber.ack(message))
				if (message.get_payload_data() == unique_payload) {
					ARE_EQUAL(std::string(message.get_exchange_name()), exchange_name)
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
