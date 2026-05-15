/**
 @file Kafka.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_Kafka_hpp
#define Chaos_Heisenbug_Kafka_hpp

#include "../Heisenbug.hpp"

#include "../Kafka/Producer.hpp"
#include "../Kafka/Consumer.hpp"

#include <chrono>
#include <random>
#include <string>

namespace chaos {
	class kafka_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		kafka_test()
		:
			heisen_test("Kafka")
		{

		};
		virtual ~kafka_test() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void initialize() override
		{

		}

		virtual void structuralize() override
		{
			HEISEN(Connection);
			HEISEN(Topic);
			HEISEN(Produce);
			HEISEN(Consume);
		}

		virtual void deinitialize() override
		{

		}
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief Producer and consumer both connect to the broker and report is_active().
		 */
		void testConnection()
		{
			kafka::producer producer(get_variable("HOST", "localhost"), get_variable("PORT", 9092));
			IS_TRUE(producer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(producer.alive(true))
			IS_TRUE(producer.is_active())
			IS_TRUE(producer.disconnect())
			IS_FALSE(producer.is_active())

			kafka::consumer consumer(get_variable("HOST", "localhost"), get_variable("PORT", 9092));
			IS_TRUE(consumer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", ""), get_variable("GROUP", "heisenberg")))
			IS_TRUE(consumer.alive(true))
			IS_TRUE(consumer.is_active())
			IS_TRUE(consumer.get_file_descriptor() > -1)
			IS_TRUE(consumer.disconnect())
			IS_FALSE(consumer.is_active())
		}

		/**
		 * @brief Verifies create_topic and delete_topic, including idempotency of both.
		 * Re-creates the topic at the end so subsequent tests can use it.
		 */
		void testTopic()
		{
			const std::string topic(get_variable("TOPIC", "heisen-test"));

			kafka::producer producer(get_variable("HOST", "localhost"), get_variable("PORT", 9092));
			IS_TRUE(producer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(producer.alive(true))

			IS_TRUE(producer.create_topic(topic, -1, -1, 5000))
			IS_TRUE(producer.create_topic(topic, -1, -1, 5000)) /// < idempotent — TOPIC_ALREADY_EXISTS treated as success

			IS_TRUE(producer.delete_topic(topic))
			IS_TRUE(producer.delete_topic(topic)) /// < idempotent — UNKNOWN_TOPIC_OR_PART treated as success

			IS_TRUE(producer.create_topic(topic, -1, -1, 5000)) /// < re-create for subsequent tests
		}

		/**
		 * @brief Producer sends a message with and without a key, then flushes.
		 */
		void testProduce()
		{
			std::default_random_engine gen;
			gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string topic(get_variable("TOPIC", "heisen-test"));
			const std::string payload("heisen-payload-" + std::to_string(std::uniform_int_distribution<std::int32_t>(100000, 999999)(gen)));

			kafka::producer producer(get_variable("HOST", "localhost"), get_variable("PORT", 9092));
			IS_TRUE(producer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(producer.alive(true))
			IS_TRUE(producer.create_topic(topic, -1, -1, 5000)) /// < idempotent — topic exists after testTopic

			IS_TRUE(producer.push(topic, payload))
			IS_TRUE(producer.push(topic, payload, "heisen-key"))
			IS_TRUE(producer.flush(5000))
		}

		/**
		 * @brief Round-trip: produce a uniquely-tagged message, then consume it.
		 *
		 * auto.offset.reset=earliest ensures the consumer reads from the beginning
		 * of the topic, so messages produced before subscribe() are visible.
		 * The test scans up to 100 messages looking for the unique payload.
		 */
		void testConsume()
		{
			std::default_random_engine gen;
			gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string topic(get_variable("TOPIC", "heisen-test"));
			const std::string unique_payload("heisen-roundtrip-" + std::to_string(std::uniform_int_distribution<std::int32_t>(100000, 999999)(gen)));

			kafka::producer producer(get_variable("HOST", "localhost"), get_variable("PORT", 9092));
			IS_TRUE(producer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			IS_TRUE(producer.alive(true))
			IS_TRUE(producer.create_topic(topic, -1, -1, 5000)) /// < idempotent — topic exists after testTopic
			IS_TRUE(producer.push(topic, unique_payload, "heisen-key"))
			IS_TRUE(producer.flush(5000))

			kafka::consumer consumer(get_variable("HOST", "localhost"), get_variable("PORT", 9092), {{"auto.offset.reset", "earliest"}});
			IS_TRUE(consumer.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", ""), get_variable("GROUP", "heisenberg") + "-roundtrip"))
			IS_TRUE(consumer.subscribe({topic}))

			bool found = false;
			for (int i = 0; i < 100 && !found; ++i) {
				kafka::message msg(consumer.poll(1000));
				if (msg.is_empty()) {
					continue;
				}
				IS_FALSE(msg.is_error())
				if (msg.get_payload() == unique_payload) {
					ARE_EQUAL(std::string(msg.get_key()), std::string("heisen-key"))
					ARE_EQUAL(std::string(msg.get_topic()), topic)
					found = true;
				}
			}
			IS_TRUE(found)
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
	/** @} */

	/** @name Formatters */
	/** @{ */
	public:
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
	/** @} */

	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
