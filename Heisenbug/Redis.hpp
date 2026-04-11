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
#include "../Redis/Command/FlushCommand.hpp"
#include "../Redis/Command/ResetStringCommand.hpp"
#include "../Redis/Command/LoadStringCommand.hpp"
#include "../Redis/Command/EmplaceStringCommand.hpp"
#include "../Redis/Command/EmplaceZetItemCommand.hpp"
#include "../Redis/Command/UpdateZetItemCommand.hpp"
#include "../Redis/Command/ResetZetItemCommand.hpp"
#include "../Redis/Command/RemoveZetItemCommand.hpp"
#include "../Redis/Command/LoadZetCommand.hpp"
#include "../Redis/Command/PruneZetCommand.hpp"
#include "../Redis/Command/ScanCommand.hpp"
#include "../Redis/Command/DeleteCommand.hpp"

#include <algorithm>
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
		virtual void initialize() override
		{

		}

		virtual void structuralize() override
		{
			HEISEN(Connection);
			HEISEN(SyncStringSet);
			HEISEN(AsyncStringSet);
			HEISEN(SyncZetSet);
			HEISEN(SyncScan);
			HEISEN(SyncDelete);
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
		 * @brief
		 */
		void testConnection()
		{
			redis::async_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("CLIENT_NAME", "heisenberg"), get_variable("DATABASE_INDEX", 8));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))
			/// @note Clean up any leftover state from previous runs
			IS_TRUE(connection.call<redis::flush_command>(false));
		}

		/**
		 * @brief
		 */
		void testSyncStringSet()
		{
			std::default_random_engine value_generator;
			value_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string test_value(std::to_string(std::uniform_int_distribution<std::int16_t>(1, 9999)(value_generator)));

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("CLIENT_NAME", "heisenberg"), get_variable("DATABASE_INDEX", 8));
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

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("CLIENT_NAME", "heisenberg"), get_variable("DATABASE_INDEX", 8));
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

		/**
		 * @brief Exercises all 6 Zet commands against a real Redis connection.
		 *
		 * Key: "test_zet"
		 * Member: random integer string, scores are positive unix-like integers.
		 */
		void testSyncZetSet()
		{
			std::default_random_engine score_generator;
			score_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::int64_t base_score(std::uniform_int_distribution<std::int32_t>(100'000, 999'999)(score_generator));
			const std::string test_member("node_" + std::to_string(base_score));

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("CLIENT_NAME", "heisenberg"), get_variable("DATABASE_INDEX", 8));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			/// @brief NX — first call succeeds, second is a no-op
			redis::command<bool> emplace1(connection.call<redis::emplace_zet_item_command>("test_zet", base_score, test_member));
			IS_TRUE(emplace1 && (*emplace1))

			redis::command<bool> emplace2(connection.call<redis::emplace_zet_item_command>("test_zet", base_score + 1, test_member));
			IS_TRUE(emplace2 && (*emplace1))

			/// @brief score range [base_score, +inf] must include test_member
			redis::command<std::vector<std::string>> load1(connection.call<redis::load_zet_command>("test_zet", base_score));
			IS_TRUE(load1)
			IS_TRUE(std::find((*load1).cbegin(), (*load1).cend(), test_member) != (*load1).cend())

			/// @brief higher score propagates
			const std::int64_t higher_score(base_score + 1000);
			redis::command<bool> update_gt(connection.call<redis::update_zet_item_command>("test_zet", higher_score, test_member));
			IS_TRUE(update_gt && (*update_gt))

			/// @brief lower score is rejected
			redis::command<bool> update_gt_reject(connection.call<redis::update_zet_item_command>("test_zet", base_score, test_member));
			IS_TRUE(update_gt_reject && !(*update_gt_reject))

			// update (LT): lower score propagates
			redis::command<bool> update_lt(connection.call<redis::update_zet_item_command>("test_zet", -base_score, test_member));
			IS_TRUE(update_lt && (*update_lt))

			/// @brief unconditional overwrite back to base_score
			redis::command<void> reset(connection.call<redis::reset_zet_item_command>("test_zet", base_score, test_member));
			IS_TRUE(reset)

			/// @brief remove everything with score <= base_score (removes our member)
			redis::command<void> prune(connection.call<redis::prune_zet_command>("test_zet", std::numeric_limits<std::int64_t>::min(), base_score));
			IS_TRUE(prune)

			/// @brief load after prune: test_member must be gone
			redis::command<std::vector<std::string>> load2(connection.call<redis::load_zet_command>("test_zet", base_score));
			IS_TRUE(load2)
			IS_TRUE(std::find((*load2).cbegin(), (*load2).cend(), test_member) == (*load2).cend())
		}

		/**
		 * @brief Exercises scan_command against a real Redis connection.
		 *
		 * Seeds known keys under a unique prefix, then scans by pattern and
		 * verifies all seeded keys are present in the result. Cleans up afterwards.
		 */
		void testSyncScan()
		{
			std::default_random_engine id_generator;
			id_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string prefix("heisen:scan:" + std::to_string(std::uniform_int_distribution<std::int32_t>(100'000, 999'999)(id_generator)));
			const std::string pattern(prefix + ":*");

			const std::vector<std::string> test_keys =
			{
				prefix + ":alpha",
				prefix + ":beta",
				prefix + ":gamma"
			};

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("CLIENT_NAME", "heisenberg"), get_variable("DATABASE_INDEX", 8));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			for (const std::string& key : test_keys) {
				connection.call<redis::reset_string_command>(key, "1");
			}

			redis::command<std::vector<std::string>> scan(connection.call<redis::scan_command>(pattern));
			IS_TRUE(scan)

			const std::vector<std::string>& found(*scan);
			for (const std::string& key : test_keys) {
				IS_TRUE(std::find(found.cbegin(), found.cend(), key) != found.cend())
			}

			redis::command<std::int32_t> cleanup(connection.call<redis::delete_command>(test_keys));
			ARE_EQUAL(*cleanup, static_cast<std::int32_t>(test_keys.size()))
		}

		/**
		 * @brief Exercises delete_command against a real Redis connection.
		 *
		 * Writes a key, deletes it, verifies it is gone.
		 * Also verifies that deleting a non-existent key returns 0.
		 */
		void testSyncDelete()
		{
			std::default_random_engine id_generator;
			id_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			const std::string test_key("heisen:del:" + std::to_string(std::uniform_int_distribution<std::int32_t>(100'000, 999'999)(id_generator)));

			redis::sync_connection connection(get_variable("HOST", "localhost"), get_variable("PORT", 6379), get_variable("CLIENT_NAME", "heisenberg"), get_variable("DATABASE_INDEX", 8));
			IS_TRUE(connection.connect(get_variable("USERNAME", ""), get_variable("PASSWORD", "")))

			/// @brief Ensure key doesn't exist, then write it
			connection.call<redis::delete_command>(test_key);
			connection.call<redis::reset_string_command>(test_key, "1");

			/// @brief First delete: key existed — expect count = 1
			redis::command<std::int32_t> del1(connection.call<redis::delete_command>(test_key));
			IS_TRUE(del1)
			ARE_EQUAL(*del1, 1)

			/// @brief Second delete: key is gone — expect count = 0
			redis::command<std::int32_t> del2(connection.call<redis::delete_command>(test_key));
			IS_TRUE(del2)
			ARE_EQUAL(*del2, 0)

			/// @brief Load after delete: command must fail (key absent → nil reply)
			redis::command<std::string> load(connection.call<redis::load_string_command>(test_key));
			IS_FALSE(load)
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
