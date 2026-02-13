/**
 @file Redis.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_Redis_hpp
#define Chaos_Heisenbug_Redis_hpp

#include "../Heisenbug.hpp"

#include "../Redis/Redis.hpp"
#include "../Redis/Connection.hpp"
#include "../Redis/Command/ResetStringCommand.hpp"
#include "../Redis/Command/LoadStringCommand.hpp"
#include "../Redis/Command/EmplaceStringCommand.hpp"

#include <random>

namespace chaos {
	class redis_test : public heisen_test
	{
	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		redis_test()
		:
			heisen_test("Redis")
		{

		};
		virtual ~redis_test() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void structuralize() override
		{
			HEISEN(Connection);
			HEISEN(SyncStringSet);
			HEISEN(AsyncStringSet);
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
		 * @brief
		 */
		void testConnection()
		{
			redis::async_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("NAME", "heisenberg"));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
		}

		/**
		 * @brief
		 */
		void testSyncStringSet()
		{
			std::default_random_engine value_generator;
			value_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string test_value(std::to_string(std::uniform_int_distribution<std::int16_t>(1, 9999)(value_generator)));

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("NAME", "heisenberg"));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			redis::command<void> reset_command(connection.call<redis::reset_string_command>("test1_key", test_value));
			IS_TRUE(reset_command)

			redis::command<std::string> load_command(connection.call<redis::load_string_command>("test1_key"));
			ARE_EQUAL(*load_command, test_value)

			redis::command<void> emplace2_1_command(connection.call<redis::emplace_string_command>("test2_key", test_value));
			IS_TRUE(emplace2_1_command)

			redis::command<void> emplace2_2_command(connection.call<redis::emplace_string_command>("test2_key", test_value));
			IS_FALSE(emplace2_2_command)

			redis::command<void> emplace3_1_command(connection.call<redis::emplace_string_command>("test3_key", test_value, std::chrono::milliseconds{20'000}));
			IS_TRUE(emplace3_1_command)

			redis::command<void> emplace3_2_command(connection.call<redis::emplace_string_command>("test3_key", test_value, std::chrono::milliseconds{20'000}));
			IS_FALSE(emplace3_2_command)
		}

		/**
		 * @brief
		 */
		void testAsyncStringSet()
		{
			std::default_random_engine value_generator;
			value_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string test_value(std::to_string(std::uniform_int_distribution<std::int16_t>(1, 9999)(value_generator)));

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("NAME", "heisenberg"));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			redis::async_connection connection1(std::move(connection));
			IS_TRUE(connection1.is_active())

			redis::async_connection connection2(std::move(connection));
			IS_FALSE(connection2.is_active())

			redis::command<void> reset_command(connection1.call<redis::reset_string_command>("test_key", test_value));
			IS_TRUE(reset_command)

			redis::command<std::string> load1_command(connection1.call<redis::load_string_command>("test_key"));
			IS_TRUE(load1_command)

			IS_TRUE(connection1.send(true))

			ARE_EQUAL(*load1_command, test_value)

			redis::command<std::string> load2_command(connection1.call<redis::load_string_command>("test_key"));
			IS_TRUE(connection1.send(true))

			ARE_EQUAL(*load1_command, *load2_command)
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
