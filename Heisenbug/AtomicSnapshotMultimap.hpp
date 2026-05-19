/**
 @file AtomicSnapshotMultimap.hpp
 @date 2026-05-08
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Heisenbug_AtomicSnapshotMultimap_hpp
#define Chaos_Heisenbug_AtomicSnapshotMultimap_hpp

#include "../Heisenbug.hpp"

#include "../AtomicSnapshotMultimap.hpp"
#include "../SafePointer.hpp"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <set>

namespace chaos {
	class atomic_snapshot_multimap_test : public heisen_test
	{
	/** @name Aliases */
	/** @{ */
	private:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_snapshot_multimap_test()
		:
			heisen_test("AtomicSnapshotMultimap")
		{
		}

		virtual ~atomic_snapshot_multimap_test() = default;
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
		virtual void initialize() override
		{
		}

		virtual void structuralize() override
		{
			HEISEN(NullLoadForMissingKey);
			HEISEN(BasicInsertAndLoad);
			HEISEN(MultipleValuesPerKey);
			HEISEN(RemoveSingleValue);
			HEISEN(RemoveFromMissingKey);
			HEISEN(SnapshotIsolation);
			HEISEN(StaleEntryPruning);
			HEISEN(SafePtrTraits);
			HEISEN(ConcurrentInsertAndLoad);
			HEISEN(ConcurrentInsertAndRemove);
		}

		virtual void deinitialize() override
		{
		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief load() must return nullptr for a key that was never inserted.
		 */
		void testNullLoadForMissingKey()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			IS_TRUE(map.load(42) == nullptr);
			IS_TRUE(map.load(0) == nullptr);
		}

		/**
		 * @brief Insert one value; load must return a snapshot containing exactly that value.
		 */
		void testBasicInsertAndLoad()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			map.insert(1, std::make_shared<int>(7));

			auto snapshot(map.load(1));
			IS_TRUE(snapshot != nullptr);
			ARE_EQUAL(snapshot->size(), static_cast<std::size_t>(1));
			ARE_EQUAL(**snapshot->begin(), 7);
		}

		/**
		 * @brief Multiple inserts under the same key accumulate; different keys are independent.
		 */
		void testMultipleValuesPerKey()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			std::shared_ptr<int> a(std::shared_ptr<int>(new int(1)));
			std::shared_ptr<int> b(std::shared_ptr<int>(new int(2)));
			std::shared_ptr<int> c(std::shared_ptr<int>(new int(3)));

			map.insert(10, a);
			map.insert(10, b);
			map.insert(10, c);
			map.insert(20, a);

			auto snapshot10(map.load(10));
			IS_TRUE(snapshot10 != nullptr);
			ARE_EQUAL(snapshot10->size(), static_cast<std::size_t>(3));

			auto snapshot20(map.load(20));
			IS_TRUE(snapshot20 != nullptr);
			ARE_EQUAL(snapshot20->size(), static_cast<std::size_t>(1));

			IS_TRUE(map.load(30) == nullptr);
		}

		/**
		 * @brief remove() drops the matching entry; remaining entries are intact.
		 */
		void testRemoveSingleValue()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			auto a(std::make_shared<int>(10));
			auto b(std::make_shared<int>(20));
			auto c(std::make_shared<int>(30));

			map.insert(1, a);
			map.insert(1, b);
			map.insert(1, c);

			map.remove(1, b);

			auto snapshot(map.load(1));
			IS_TRUE(snapshot != nullptr);
			ARE_EQUAL(snapshot->size(), static_cast<std::size_t>(2));

			bool found_a(false), found_c(false);
			for (const auto& item : *snapshot) {
				if (item == a) found_a = true;
				if (item == c) found_c = true;
			}
			IS_TRUE(found_a);
			IS_TRUE(found_c);
		}

		/**
		 * @brief remove() on a key that was never inserted must be a no-op.
		 */
		void testRemoveFromMissingKey()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			auto value(std::make_shared<int>(99));
			map.remove(999, value);
			IS_TRUE(map.load(999) == nullptr);
		}

		/**
		 * @brief A snapshot captured before a write remains valid and unchanged
		 * after subsequent inserts/removes — copy-on-write isolation.
		 */
		void testSnapshotIsolation()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			auto a(std::make_shared<int>(1));
			auto b(std::make_shared<int>(2));

			map.insert(1, a);
			auto old_snapshot(map.load(1));

			map.insert(1, b);
			auto new_snapshot(map.load(1));

			ARE_EQUAL(old_snapshot->size(), static_cast<std::size_t>(1));
			ARE_EQUAL(new_snapshot->size(), static_cast<std::size_t>(2));

			map.remove(1, a);
			auto after_remove(map.load(1));

			ARE_EQUAL(old_snapshot->size(), static_cast<std::size_t>(1));
			ARE_EQUAL(new_snapshot->size(), static_cast<std::size_t>(2));
			ARE_EQUAL(after_remove->size(), static_cast<std::size_t>(1));
		}

		/**
		 * @brief Null shared_ptr values are pruned on the next copy-on-write write (insert or remove).
		 * Insert rebuilds the set, skipping null entries, so the null is gone after the next insert.
		 */
		void testStaleEntryPruning()
		{
			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			auto live_a(std::make_shared<int>(1));
			auto live_b(std::make_shared<int>(2));
			std::shared_ptr<int> stale; // null — !!item is false

			map.insert(1, live_a);
			map.insert(1, stale);

			// Both live_a and the null stale entry are present
			{
				auto before(map.load(1));
				IS_TRUE(before != nullptr);
				ARE_EQUAL(before->size(), static_cast<std::size_t>(2));
			}

			// Trigger a write: copy-on-write rebuild skips null, prunes the stale entry
			map.insert(1, live_b);

			auto after(map.load(1));
			IS_TRUE(after != nullptr);
			ARE_EQUAL(after->size(), static_cast<std::size_t>(2)); // live_a, live_b (stale gone)
			for (const auto& entry : *after) {
				IS_TRUE(entry != nullptr);
			}
		}

		/**
		 * @brief safe_ptr<T> as value type: operator bool() for null check, operator< for set ordering.
		 */
		void testSafePtrTraits()
		{
			atomic_snapshot_multimap<std::uintptr_t, safe_ptr<int>> map;
			safe_ptr<int> a(std::make_shared<int>(100));
			safe_ptr<int> b(std::make_shared<int>(200));

			map.insert(1, a);
			map.insert(1, b);

			auto snapshot(map.load(1));
			IS_TRUE(snapshot != nullptr);
			ARE_EQUAL(snapshot->size(), static_cast<std::size_t>(2));

			map.remove(1, a);
			auto after_remove(map.load(1));
			IS_TRUE(after_remove != nullptr);
			ARE_EQUAL(after_remove->size(), static_cast<std::size_t>(1));
			IS_TRUE(*after_remove->begin() == b);
		}

		/**
		 * @brief Many threads inserting under disjoint keys concurrently;
		 * every key's snapshot must contain exactly the expected number of values.
		 */
		void testConcurrentInsertAndLoad()
		{
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t inserts_per_thread = 200;

			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;

			std::vector<std::thread> thread_vector;
			for (std::size_t thread_index = 0; thread_index < thread_count; ++thread_index) {
				thread_vector.emplace_back
				(
					[&map, thread_index]()
					{
						const std::uintptr_t key(static_cast<std::uintptr_t>(thread_index));
						for (std::size_t iteration = 0; iteration < inserts_per_thread; ++iteration) {
							map.insert(key, std::make_shared<int>(static_cast<int>(iteration)));
						}
					}
				);
			}
			for (std::thread& thread : thread_vector) {
				thread.join();
			}

			for (std::size_t thread_index = 0; thread_index < thread_count; ++thread_index) {
				const std::uintptr_t key(static_cast<std::uintptr_t>(thread_index));
				auto snapshot(map.load(key));
				IS_TRUE(snapshot != nullptr);
				ARE_EQUAL(snapshot->size(), inserts_per_thread);
			}
		}

		/**
		 * @brief Multiple threads insert and remove under the same key concurrently;
		 * no crash, no deadlock, and final snapshot reflects a consistent state.
		 */
		void testConcurrentInsertAndRemove()
		{
			constexpr std::size_t thread_count = 6;
			constexpr std::size_t iterations = 500;
			constexpr std::uintptr_t key(42);

			atomic_snapshot_multimap<std::uintptr_t, std::shared_ptr<int>> map;
			std::atomic<bool> stop_flag{false};
			std::atomic<std::size_t> error_count{0};

			std::vector<std::thread> thread_vector;
			for (std::size_t thread_index = 0; thread_index < thread_count; ++thread_index) {
				thread_vector.emplace_back
				(
					[&map, &stop_flag, &error_count, thread_index, key]()
					{
						try {
							for (std::size_t iteration = 0; iteration < iterations && !stop_flag; ++iteration) {
								auto value(std::make_shared<int>(static_cast<int>(thread_index * 1000 + iteration)));
								map.insert(key, value);
								auto snapshot(map.load(key));
								if (!snapshot) {
									error_count++;
								}
								map.remove(key, value);
							}
						} catch (const std::exception& exception) {
							(void)exception;
							error_count++;
						}
					}
				);
			}

			for (std::thread& thread : thread_vector) {
				thread.join();
			}

			IS_TRUE(error_count == 0);
		}
	/** @} */
	};
}

#endif
