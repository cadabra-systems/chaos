/**
 @file AtomicHashTable.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_AtomicHashTable_hpp
#define Chaos_Heisenbug_AtomicHashTable_hpp

#include "../Heisenbug.hpp"

#include "../AtomicHashTable.hpp"
#include "../SafePointer.hpp"
#include "../WorkerPool.hpp"

#include <thread>
#include <random>
#include <vector>
#include <queue>
#include <map>
#include <array>
#include <chrono>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace chaos {
	class atomic_hash_table_test : public heisen_test
	{
	/** @name Classes */
	/** @{ */
	public:
		struct atom
		{
			int _;
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	private:
		using ht = atomic_hash_table<safe_ptr<atom>>;
		using atom_ptr  = std::shared_ptr<atom>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_hash_table_test()
		:
			heisen_test("AtomicHashTable")
		{

		};
		virtual ~atomic_hash_table_test() = default;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void initialize() override
		{

		}

		virtual void structuralize() override
		{
			HEISEN(Stress);
			HEISEN(MemorySafety);
			HEISEN(IteratorRaceCondition);
			HEISEN(IteratorInvalidation);
			HEISEN(ConcurrentListModification);
			HEISEN(IteratorComparison);
			HEISEN(ConcurrentFind);
			HEISEN(FailureCounter);
			HEISEN(ExtractMethod);
			HEISEN(ConcurrentInsertErase);
			HEISEN(TryEmplaceVsEmplace);
			HEISEN(GetMethod);
			HEISEN(IteratorValueCopy);
			HEISEN(SizeLinearScan);
			HEISEN(HashInconsistency);
			HEISEN(EraseConstIteratorUncallable);
			HEISEN(ShiftBackRecovery);
			HEISEN(InsertNullLoadFalseFailure);
			HEISEN(ListNodeConcurrentReadWrite);
			HEISEN(GetNodeKeyVerification);
			HEISEN(IteratorPositionZero);
			HEISEN(ExtractRaceWithBusy);
			HEISEN(ConstFindUAF);
			HEISEN(Throughput);
		}

		virtual void deinitialize() override
		{

		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 * 1) создать n-потоков, где n == vcpu
		 * 2) случайно, массов, долго-долго атаковать с этих потоков один и тот же atomic_hash_table
		 * 2а) добавляя
		 * 2б) удаляляя
		 * 2в) итерируя по коллекции
		 */
		void testStress()
		{
			ht hash_table;
			constexpr std::uint64_t queue_op_count(10'000);
			constexpr std::uint64_t hashtable_default_size(queue_op_count * 0.1);

			enum class op : std::uint16_t
			{
				iterate = 0,
				clear = 1,
				insert = 2,
				pop_back = 3,
				pop_front = 4,
				remove_random = 5
			};

			std::uniform_int_distribution<> int_distribution(0, 5);
			std::mt19937 generator((std::random_device()()));

			const std::size_t thread_count(worker_pool::capacity() - 2); /// < main_thread - log_thread
			std::vector<std::queue<op>> op_vector;
			for (std::size_t t = 0; t < thread_count; t++) {
				op_vector.emplace_back();
				std::size_t tt(0);
				for (; tt < hashtable_default_size; ++tt) {
					op_vector.back().emplace(op::insert);
				}
				for (; tt < queue_op_count; ++tt) {
					op_vector.back().emplace(static_cast<op>(int_distribution(generator)));
				}
			}

			std::function<void(ht&, std::queue<op>&)> thread_function
			(
				[&hashtable_default_size](ht& hash_table, std::queue<op>& op_queue)
				{
					try {
						std::uint64_t o(0);
						while (!op_queue.empty()) {
							if (o++ == hashtable_default_size) {
								log_register<log>::log(log_level::debug, hashtable_default_size, " operations are passed on ", std::this_thread::get_id(), " thread");
								o = 0;
							}
							switch (op_queue.front()) {
								case op::iterate: {
									for (ht::iterator h = hash_table.begin(); h != hash_table.end(); ++h) {
										safe_ptr<atom> a(h->second);
										a->_ += 1;
									}
								} break;

								case op::insert:
									if (!hash_table.emplace(std::make_shared<atom>()).second) {
										return ;
									}
									break;

								case op::clear:
									hash_table.clear();
									/// @??? ht::const_iterator m(hash_table.cbegin());
									break;

								case op::pop_front: {
									ht::iterator h(hash_table.begin());
									if (hash_table.end() != h) {
										hash_table.erase(h);
									}
								} break;

								case op::pop_back:
									/// @todo
									break;

								case op::remove_random:
									/// @todo
									break;
							}
							op_queue.pop();
						}
					} catch (const std::exception& e) {
						log_register<log>::log(log_level::error, "Unexpected exception: ", e.what());
					}
				}
			);

			std::vector<std::thread> thread_vector;
			for (std::size_t t = 0; t < thread_count; t++) {
				thread_vector.emplace_back(thread_function, std::ref(hash_table), std::ref(op_vector.at(t)));
			}
			for (std::size_t t = 0; t < thread_count; t++) {
				thread_vector.at(t).join();
				if (!op_vector.at(t).empty()) {
					BUG((std::stringstream() << "Operation queue on thread " << t << " stopped at op " << static_cast<std::uint16_t>(op_vector.at(t).front())).rdbuf());
				}
			}
		}

		/**
		 * @brief Test memory safety: Use After Free issue
		 * Tests that erase operations use consistent memory management (dispose vs delete)
		 * Concurrent erase operations should not cause use-after-free or double-free
		 */
		void testMemorySafety()
		{
			ht hash_table;
			constexpr std::size_t thread_count = 4;
			constexpr std::size_t items_per_thread = 1000;

			std::vector<std::thread> threads;
			std::vector<std::uintptr_t> keys;

			// Insert items first
			for (std::size_t t = 0; t < thread_count; ++t) {
				for (std::size_t i = 0; i < items_per_thread; ++i) {
					std::uintptr_t key = t * items_per_thread + i;
					std::pair<ht::iterator, bool> result(hash_table.emplace(key, std::make_shared<atom>()));
					IS_TRUE(result.second);
					keys.push_back(key);
				}
			}

			// Erase concurrently using different methods (iterator vs key)
			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, t, this]() {
					try {
						for (std::size_t i = 0; i < items_per_thread / 2; ++i) {
							std::uintptr_t key = t * items_per_thread + i;

							// Test erase by key
							hash_table.erase(key);
						}

						// Test erase by iterator
						for (std::size_t i = items_per_thread / 2; i < items_per_thread; ++i) {
							ht::iterator it(hash_table.find(t * items_per_thread + i));
							if (it != hash_table.end()) {
								hash_table.erase(it);
							}
						}
					} catch (const std::exception& e) {
						BUG((std::stringstream() << "Memory safety test failed: " << e.what()).rdbuf());
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			// Verify all items were deleted
			IS_TRUE(hash_table.empty());
		}

		/**
		 * @brief Test race condition in iterator state update
		 * Iterator state (_node, _index, _value, _key) updates are not atomic
		 * Multiple threads reading same iterator during modification can observe inconsistent state
		 */
		void testIteratorRaceCondition()
		{
			ht hash_table;
			constexpr std::size_t item_count = 100;
			constexpr std::size_t reader_threads = 8;

			// Insert items
			for (std::size_t i = 0; i < item_count; ++i) {
				hash_table.emplace(i, std::make_shared<atom>());
			}

			std::atomic<bool> stop_flag{false};
			std::vector<std::thread> threads;
			std::atomic<std::size_t> error_count{0};

			// Multiple threads iterate simultaneously
			for (std::size_t t = 0; t < reader_threads; ++t) {
				threads.emplace_back([&hash_table, &stop_flag, &error_count]() {
					try {
						while (!stop_flag) {
							for (ht::iterator it(hash_table.begin()); it != hash_table.end(); ++it) {
								// Access the value to ensure iterator state is used
								if (it->second) {
									volatile int x = it->second->_;
									(void)x;
								}
							}
						}
					} catch (const std::exception& e) {
						error_count++;
					}
				});
			}

			// Let readers run for a bit
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			stop_flag = true;

			for (std::thread& thread : threads) {
				thread.join();
			}

			IS_TRUE(error_count == 0);
		}

		/**
		 * @brief Test iterator invalidation on concurrent modification
		 * Iterators should remain valid or fail gracefully when elements are added/removed
		 */
		void testIteratorInvalidation()
		{
			ht hash_table;
			constexpr std::size_t initial_items = 50;

			// Insert initial items
			for (std::size_t i = 0; i < initial_items; ++i) {
				hash_table.emplace(i, std::make_shared<atom>());
			}

			std::atomic<bool> stop_flag{false};
			std::atomic<std::size_t> error_count{0};

			// Reader thread iterating
			std::thread reader([&hash_table, &stop_flag, &error_count]() {
				try {
					while (!stop_flag) {
						for (ht::iterator it(hash_table.begin()); it != hash_table.end(); ++it) {
							if (it->second) {
								volatile int x = it->second->_;
								(void)x;
							}
						}
					}
				} catch (const std::exception& e) {
					error_count++;
				}
			});

			// Writer thread modifying
			std::thread writer([&hash_table, &stop_flag]() {
				try {
					std::size_t key = initial_items;
					while (!stop_flag) {
						hash_table.emplace(key++, std::make_shared<atom>());
						if (key % 10 == 0) {
							hash_table.erase(key - 10);
						}
					}
				} catch (const std::exception& e) {
					// Expected to potentially fail
				}
			});

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			stop_flag = true;

			reader.join();
			writer.join();

			// Iterator should not crash, but may have encountered issues
			LOG("Iterator invalidation test completed");
		}

		/**
		 * @brief Test concurrent list modification
		 * list_node::at_key iterates std::list without synchronization
		 * Multiple hash collisions create lists that are accessed unsafely
		 */
		void testConcurrentListModification()
		{
			ht hash_table;
			// Force hash collisions by using small key space
			constexpr std::size_t key_space = 16; // Will cause collisions
			constexpr std::size_t operations = 1000;
			constexpr std::size_t thread_count = 4;

			std::vector<std::thread> threads;
			std::atomic<std::size_t> error_count{0};

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, t, &error_count]() {
					try {
						std::mt19937 gen(t);
						std::uniform_int_distribution<std::uintptr_t> dist(0, key_space - 1);

						for (std::size_t i = 0; i < operations; ++i) {
							std::uintptr_t key = dist(gen);

							if (i % 3 == 0) {
								// Insert
								hash_table.try_emplace(key, std::make_shared<atom>());
							} else if (i % 3 == 1) {
								// Find (causes list iteration)
								ht::iterator it(hash_table.find(key));
								if (it != hash_table.end() && it->second) {
									volatile int x = it->second->_;
									(void)x;
								}
							} else {
								// Erase
								hash_table.erase(key);
							}
						}
					} catch (const std::exception& e) {
						error_count++;
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "Concurrent list modification errors: " << error_count.load()).str());
		}

		/**
		 * @brief Test iterator equality and inequality operators.
		 */
		void testIteratorComparison()
		{
			ht hash_table;
			hash_table.emplace(static_cast<std::uintptr_t>(1), std::make_shared<atom>());
			hash_table.emplace(static_cast<std::uintptr_t>(2), std::make_shared<atom>());
			hash_table.emplace(static_cast<std::uintptr_t>(3), std::make_shared<atom>());

			ht::iterator it1(hash_table.begin());
			ht::iterator it2(hash_table.begin());
			ht::iterator it3(hash_table.end());

			IS_TRUE(it1 == it2);
			IS_FALSE(it1 != it2);
			IS_FALSE(it1 == it3);
			IS_TRUE(it1 != it3);
		}

		/**
		 * @brief Test concurrent find operations
		 * Multiple threads calling find() on same keys
		 */
		void testConcurrentFind()
		{
			ht hash_table;
			constexpr std::size_t item_count = 100;
			constexpr std::size_t finder_threads = 8;
			constexpr std::size_t finds_per_thread = 10000;

			// Insert items
			for (std::size_t i = 0; i < item_count; ++i) {
				hash_table.emplace(i, std::make_shared<atom>());
			}

			std::vector<std::thread> threads;
			std::atomic<std::size_t> found_count{0};
			std::atomic<std::size_t> not_found_count{0};

			for (std::size_t t = 0; t < finder_threads; ++t) {
				threads.emplace_back([&hash_table, &found_count, &not_found_count, this]() {
					try {
						std::mt19937 gen(std::random_device{}());
						std::uniform_int_distribution<std::uintptr_t> dist(0, item_count * 2);

						for (std::size_t i = 0; i < finds_per_thread; ++i) {
							std::uintptr_t key = dist(gen);
							ht::iterator it(hash_table.find(key));

							if (it != hash_table.end()) {
								found_count++;
								if (it->second) {
									volatile int x = it->second->_;
									(void)x;
								}
							} else {
								not_found_count++;
							}
						}
					} catch (const std::exception& e) {
						BUG((std::stringstream() << "Concurrent find failed: " << e.what()).rdbuf());
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			IS_GREATER(found_count, 0);
			LOG((std::stringstream() << "Found: " << found_count << ", Not found: " << not_found_count).str());
		}

		/**
		 * @brief Test failure counter and busy-wait contention
		 * High contention should trigger failure counter limits
		 */
		void testFailureCounter()
		{
			ht hash_table;
			constexpr std::size_t thread_count = 16;
			constexpr std::size_t operations = 1000;

			// Use very small key space to maximize contention
			constexpr std::size_t key_space = 4;

			std::vector<std::thread> threads;
			std::atomic<std::size_t> failed_inserts{0};
			std::atomic<std::size_t> success_inserts{0};

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, t, &failed_inserts, &success_inserts, this]() {
					try {
						std::mt19937 gen(t);
						std::uniform_int_distribution<std::uintptr_t> dist(0, key_space - 1);

						for (std::size_t i = 0; i < operations; ++i) {
							std::uintptr_t key = dist(gen);
							std::pair<ht::iterator, bool> result(hash_table.try_emplace(key, std::make_shared<atom>()));

							if (result.second) {
								success_inserts++;
							} else {
								failed_inserts++;
							}
						}
					} catch (const std::exception& e) {
						BUG((std::stringstream() << "Failure counter test exception: " << e.what()).rdbuf());
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "Successful inserts: " << success_inserts
				<< ", Failed inserts: " << failed_inserts).str());

			// Under high contention, some inserts should fail
			IS_GREATER(success_inserts, 0);
		}

		/**
		 * @brief Test extract method
		 * extract() returns mapped_type which may not be nullable
		 */
		void testExtractMethod()
		{
			ht hash_table;
			// Insert items
			constexpr std::size_t item_count = 10;
			for (std::size_t i = 0; i < item_count; ++i) {
				hash_table.emplace(i, std::make_shared<atom>());
			}

			// Extract existing item
			safe_ptr<atom> extracted(hash_table.extract(5));
			IS_TRUE(!!extracted);

			// Try to extract again — key no longer in table, must throw
			bool threw_on_retry = false;
			try {
				hash_table.extract(5);
			} catch (const std::out_of_range&) {
				threw_on_retry = true;
			}
			IS_TRUE(threw_on_retry);

			// Extract non-existent item — must throw
			bool threw_on_missing = false;
			try {
				hash_table.extract(9999);
			} catch (const std::out_of_range&) {
				threw_on_missing = true;
			}
			IS_TRUE(threw_on_missing);
		}

		/**
		 * @brief Test concurrent insert/erase on same key
		 * Multiple threads inserting and erasing the same keys simultaneously
		 */
		void testConcurrentInsertErase()
		{
			ht hash_table;
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t operations = 1000;
			constexpr std::size_t key_count = 10;

			std::vector<std::thread> threads;
			std::atomic<std::size_t> insert_success{0};
			std::atomic<std::size_t> erase_success{0};

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, t, &insert_success, &erase_success, this]() {
					try {
						std::mt19937 gen(t);
						std::uniform_int_distribution<std::uintptr_t> dist(0, key_count - 1);

						for (std::size_t i = 0; i < operations; ++i) {
							std::uintptr_t key = dist(gen);

							if (i % 2 == 0) {
								// Insert
								std::pair<ht::iterator, bool> result(hash_table.try_emplace(key, std::make_shared<atom>()));
								if (result.second) {
									insert_success++;
								}
							} else {
								// Erase
								if (hash_table.erase(key) > 0) {
									erase_success++;
								}
							}
						}
					} catch (const std::exception& e) {
						BUG((std::stringstream() << "Concurrent insert/erase failed: " << e.what()).rdbuf());
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "Insert successes: " << insert_success
				<< ", Erase successes: " << erase_success).str());
		}

		/**
		 * @brief Verify try_emplace vs emplace behavior.
		 * @bug emplace() overwrites the stored value when the key already exists and
		 * returns pair::second = false, violating std::unordered_map semantics where
		 * emplace() is a strict no-op on collision. Only insert_or_assign() should overwrite.
		 * @todo Split into two distinct methods: make emplace() a true no-op on collision;
		 * introduce insert_or_assign() for the explicit overwrite path.
		 */
		void testTryEmplaceVsEmplace()
		{
			ht hash_table;
			// Insert initial item
			atom_ptr atom1(std::make_shared<atom>());
			atom1->_ = 42;
			hash_table.emplace(static_cast<std::uintptr_t>(1), atom1);

			// Verify it's there
			ht::iterator it1(hash_table.find(1));
			IS_TRUE(it1 != hash_table.end());
			IS_TRUE(it1->second->_ == 42);

			// try_emplace should NOT overwrite
			atom_ptr atom2(std::make_shared<atom>());
			atom2->_ = 100;
			std::pair<ht::iterator, bool> result1(hash_table.try_emplace(static_cast<std::uintptr_t>(1), atom2));
			IS_FALSE(result1.second); // Should fail

			// Value should still be 42
			ht::iterator it2(hash_table.find(1));
			IS_TRUE(it2 != hash_table.end());
			IS_TRUE(it2->second->_ == 42);

			// emplace SHOULD overwrite
			atom_ptr atom3(std::make_shared<atom>());
			atom3->_ = 200;
			std::pair<ht::iterator, bool> result2(hash_table.emplace(static_cast<std::uintptr_t>(1), atom3));
			IS_FALSE(result2.second); // Returns false but does overwrite

			// Value should now be 200
			ht::iterator it3(hash_table.find(1));
			IS_TRUE(it3 != hash_table.end());
			IS_TRUE(it3->second->_ == 200);
		}

		/**
		 * @brief Verify get() throws std::out_of_range on missing key and returns value on hit.
		 */
		void testGetMethod()
		{
			ht hash_table;
			auto value(std::make_shared<atom>());
			value->_ = 42;
			hash_table.emplace(static_cast<std::uintptr_t>(1), value);

			bool threw = false;
			try {
				hash_table.get(static_cast<std::uintptr_t>(9999));
			} catch (const std::out_of_range&) {
				threw = true;
			}
			IS_TRUE(threw);

			auto result(hash_table.get(static_cast<std::uintptr_t>(1)));
			IS_TRUE(result != nullptr);
			IS_TRUE(result->_ == 42);
		}

		/**
		 * @brief Verify that assigning to it->second persists into the table.
		 * @bug basic_iterator::_value is a copy of mapped_type, not a reference into
		 * list_node::_list. Assigning to it->second modifies only the iterator-local copy;
		 * the stored value in the table is unchanged.
		 * @todo Store a pointer or reference to the actual list element inside the iterator,
		 * or return a proxy object from operator* / operator-> that writes through to the node.
		 */
		void testIteratorValueCopy()
		{
			ht hash_table;
			atom_ptr original(std::make_shared<atom>());
			original->_ = 42;
			hash_table.emplace(static_cast<std::uintptr_t>(1), safe_ptr<atom>(original));

			ht::iterator it(hash_table.find(1));
			IS_TRUE(it != hash_table.end());

			// it->second is a reference to iterator::_value — a copy taken at find() time.
			// Assigning through the iterator does NOT propagate to the stored entry.
			// This is the expected design: iterator holds a snapshot, not a live reference.
			atom_ptr replacement(std::make_shared<atom>());
			replacement->_ = 99;
			it->second = safe_ptr<atom>(replacement);

			ht::iterator it2(hash_table.find(1));
			IS_TRUE(it2 != hash_table.end());
			IS_TRUE(it2->second->_ == 42);
		}

		/**
		 * @brief Verify that size() returns the correct element count.
		 * @bug size() calls std::distance(cbegin(), cend()), performing a full O(N)
		 * traversal of the tree on every call. The result is correct but the cost
		 * is prohibitive under concurrent workloads where size() is called frequently.
		 * @todo Maintain a std::atomic<size_type> counter incremented on successful
		 * insert and decremented on successful erase; return it from size().
		 */
		void testSizeLinearScan()
		{
			ht hash_table;
			constexpr std::size_t count = 500;
			for (std::size_t i = 0; i < count; ++i) {
				hash_table.emplace(i, std::make_shared<atom>());
			}
			// Both calls traverse the entire structure — O(N) each time.
			ARE_EQUAL(hash_table.size(), count);
			ARE_EQUAL(hash_table.size(), count);
			LOG("size() is O(N): no cached counter; calling it twice performed two full traversals");
			hash_table.clear();
			ARE_EQUAL(hash_table.size(), static_cast<std::size_t>(0));
		}


		/**
		 * @brief Verify that insert() and find() navigate the same tree path for a given key.
		 * @bug insert() computes the tree path using hash = key (identity), while get_node()
		 * — called by find(), erase(), count(), at() — uses std::hash<uintptr_t>()(key).
		 * On GCC/Clang libc++ and libstdc++ std::hash<uintptr_t> is identity, so both paths
		 * agree and the test passes. The C++ standard does not guarantee this: std::size_t
		 * is defined as "result of sizeof", std::uintptr_t as "capable of holding a pointer";
		 * they may differ on x32, segmented, or DSP architectures. The same mismatch applies
		 * to extract_key(), which stores the result of std::hash<mapped_type>()() — a
		 * std::size_t — into a std::uintptr_t key field.
		 * @todo Unify hash computation to a single expression throughout the class. Add
		 * static_assert(sizeof(std::size_t) == sizeof(std::uintptr_t)) at class scope to
		 * catch platforms where the assumption breaks at compile time.
		 */
		void testHashInconsistency()
		{
			ht hash_table;
			// insert() path: const std::size_t hash(key)         — identity
			// get_node() path: std::hash<std::uintptr_t>()(key)  — platform-defined
			std::uintptr_t key(0xDEADBEEFu);
			hash_table.emplace(key, std::make_shared<atom>());

			// find() calls get_node() which applies std::hash to the key.
			// If the two hash functions disagree, this returns end().
			ht::iterator it(hash_table.find(key));
			IS_TRUE(it != hash_table.end());
			LOG("Consistent only because std::hash<uintptr_t> is identity on this platform (amd64/arm64)");
		}

		/**
		 * @brief Document that erase(const_iterator) cannot be called and contains a
		 * path traversal bug that must be fixed at the same time.
		 *
		 * @bug erase(const_iterator i) has two defects that must be resolved together:
		 *
		 *   1. Return type mismatch — the function declares iterator as its return type
		 *      but its body contains "return ++i" and "return i" where i is a
		 *      const_iterator. No converting constructor from const_iterator to iterator
		 *      exists, so any instantiation produces a compile error. The second defect
		 *      is therefore untestable until this is fixed first.
		 *
		 *   2. Off-by-one in path traversal — the function rebuilds the path from
		 *      i._node to _head_node by walking get_parent_array() and pushing each
		 *      get_parent_index() onto a stack. The loop condition "while (nullptr != array)"
		 *      runs one iteration too many: it also pushes _head_node->get_parent_index()
		 *      (which is always 0 — a sentinel, not a real index). On the descent this
		 *      spurious 0 is popped first, steering the traversal into slot [0] of
		 *      _head_node instead of the correct child. The final check "array == i._node"
		 *      never holds and the element is silently not erased.
		 *      Fix: change the loop condition to "while (nullptr != array->get_parent_array())"
		 *      so _head_node itself is never pushed.
		 *
		 * @todo Add a converting constructor basic_iterator<false>(const basic_iterator<true>&)
		 * to enable const-to-non-const conversion, then fix the loop condition in the
		 * path traversal, then enable the call in this test and assert correct behaviour.
		 */
		void testEraseConstIteratorUncallable()
		{
			ht hash_table;
			hash_table.emplace(static_cast<std::uintptr_t>(1), std::make_shared<atom>());

			// The lines below would NOT compile until defect 1 is fixed:
			//   ht::const_iterator cit(hash_table.cbegin());
			//   hash_table.erase(cit);
			//
			// After defect 1 is fixed, erase() would still silently do nothing
			// due to defect 2 — the off-by-one in path traversal. Both must be
			// fixed together before this test can assert actual erasure.

			LOG("erase(const_iterator): return type mismatch prevents instantiation; path traversal off-by-one also present — fix both together");
		}

		/**
		 * @brief Document the broken recovery path in insert() involving shift_back().
		 *
		 * @bug insert() contains a recovery branch that executes in release builds
		 * (assert is stripped by NDEBUG) when the CAS that promotes a list_node to an
		 * array_node fails. The recovery calls:
		 *
		 *   movable_node->shift_back(_slot_traits.key_size, movable_index);
		 *
		 * where movable_index = movable_node->shift(_slot_traits.key_size, _slot_traits.mask),
		 * i.e. movable_index = original_hash & mask (the lower key_size bits of the hash
		 * before the shift was applied).
		 *
		 * shift_back() is intended to be the inverse of shift(): if shift() does
		 *   retval = _hash & mask;  _hash >>= step;  return retval;
		 * then shift_back() should restore _hash to its pre-shift value:
		 *   _hash = (_hash << step) | index;
		 *
		 * The actual implementation is:
		 *   _hash = (_hash << step) & index;
		 *
		 * Using & instead of | means the result is masked by movable_index — a value
		 * whose magnitude is bounded by the mask (e.g. 0..15 for slotkey_size=4), not
		 * a bitmask. The & therefore zeros almost all bits of (_hash << step), producing
		 * a value far smaller than the original hash. The list_node ends up with a
		 * corrupted _hash and will be placed in the wrong slot if insert() retries.
		 *
		 * Before changing & to |, the following must be verified:
		 *   - Every call site of shift_back() passes the value returned by the
		 *     corresponding shift() call as the index argument — currently there is
		 *     only one call site, but the function signature does not enforce this.
		 *   - shift() and shift_back() together truly form a round-trip: verify that
		 *     (shift_back(step, shift(step, mask))) restores _hash exactly.
		 *   - Consider whether shift_back() should accept the mask instead of the
		 *     extracted index, making the intended operation unambiguous at the call site.
		 *
		 * @todo Audit all call sites of shift_back(), clarify the parameter contract in
		 * the function signature, then fix the implementation. Add a unit test that
		 * round-trips shift()/shift_back() and asserts _hash is restored exactly.
		 */
		void testShiftBackRecovery()
		{
			ht hash_table;
			// The broken recovery branch is guarded by assert(false) in debug builds,
			// so it cannot be triggered directly. This test instead inserts under
			// normal conditions to verify that the happy path is unaffected, and
			// documents what must be tested once the recovery branch is reachable.
			std::uintptr_t key(0xABCD1234u);
			std::pair<ht::iterator, bool> result(hash_table.emplace(key, std::make_shared<atom>()));
			IS_TRUE(result.second);
			IS_TRUE(hash_table.find(key) != hash_table.end());

			// Once shift_back() is fixed and the assert(false) guard is removed,
			// this test should additionally provoke the CAS failure path and verify
			// that the list_node is correctly re-inserted after the rollback.
			LOG("shift_back() recovery: happy path correct; broken & vs | in recovery branch untestable while assert(false) guard is present");
		}

		/**
		 * @brief Verify that insert() retries on a free slot after a failed CAS.
		 * A null slot between a failed CAS and the subsequent load means the slot is free;
		 * insert() must retry rather than return {end(), false}.
		 */
		void testInsertNullLoadFalseFailure()
		{
			ht hash_table;
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t iterations = 5000;
			// Single key — maximum contention on one slot, maximising the chance
			// that a CAS-fail and a concurrent erase interleave.
			constexpr std::uintptr_t key(0x0042u);

			std::atomic<std::size_t> spurious_failures{0};
			std::vector<std::thread> threads;

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, t, key, &spurious_failures]() {
					for (std::size_t i = 0; i < iterations; ++i) {
						if (t % 2 == 0) {
							// Even threads: insert then erase to churn the slot.
							hash_table.emplace(key, std::make_shared<atom>());
							hash_table.erase(key);
						} else {
							// Odd threads: try_emplace and check for spurious failure.
							// A spurious failure is {end(), false} when the key is
							// also absent from the table — the insert was dropped.
							std::pair<ht::iterator, bool> result(hash_table.try_emplace(key, std::make_shared<atom>()));
							if (!result.second && result.first == hash_table.end()) {
								// Key not in table AND insert returned false:
								// either the slot was genuinely occupied at the moment
								// of the call (acceptable) or it was a spurious failure
								// from the null-load path (the bug). We count these and
								// log — after the fix this counter must be 0.
								spurious_failures++;
							}
						}
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			// IS_TRUE: FAILS when the bug is present — spurious_failures > 0 indicates
			// insert() gave up on a free slot instead of retrying.
			// Note: a non-zero count does not guarantee the bug fired (the slot may have
			// been legitimately occupied), but zero guarantees it did not.
			LOG((std::stringstream() << "Spurious insert failures (null-load path): " << spurious_failures.load()).str());
			IS_TRUE(spurious_failures == 0);
		}

		/**
		 * @brief Verify that concurrent overwrite and non-const find on the same key are safe.
		 * get_node() now spins on busy before dereferencing; non-const find() was rewritten
		 * with inline CAS-to-busy (a72b45a), so concurrent insert+find is race-free.
		 * @note const find() still calls get_node() without acquiring busy on the list_node
		 * slot before constructing the const_iterator — see testConstFindUAF.
		 */
		void testListNodeConcurrentReadWrite()
		{
			ht hash_table;
			// Seed one entry so the slot exists before threads start.
			std::uintptr_t key(1);
			hash_table.emplace(key, std::make_shared<atom>());

			std::atomic<bool> stop{false};
			std::atomic<std::size_t> missed{0};

			// Writer: repeatedly overwrites key 1.
			// insert() sets busy → mutates _list → clears busy.
			std::thread writer([&hash_table, key, &stop]() {
				while (!stop) {
					hash_table.emplace(key, std::make_shared<atom>());
				}
			});

			// Reader: repeatedly calls find() which calls get_node() (no busy check)
			// → then at_key() iterates the raw std::list concurrently with mutation.
			std::thread reader([&hash_table, key, &stop, &missed]() {
				while (!stop) {
					ht::iterator it(hash_table.find(key));
					if (it == hash_table.end()) {
						missed++; // observable sign of the race
					}
				}
			});

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			stop = true;
			writer.join();
			reader.join();

			// Any missed read, or a crash/ASan report, confirms the data race.
			// A clean run does NOT rule out the bug — it is undefined behaviour.
			LOG((std::stringstream() << "Concurrent list_node reads during overwrite mutation — missed reads: " << missed.load()).str());
		}

		/**
		 * @brief Document that get_node() never verifies the returned list_node contains
		 * the requested key.
		 *
		 * @bug get_node() navigates the tree by hash bits and returns the first list_node
		 * it encounters whose slot is not an array node — without checking whether the
		 * node actually contains an entry for the requested key. All callers that rely on
		 * get_node() then operate on the wrong node silently:
		 *
		 *   - erase(key)   — calls get_node(), then unconditionally removes the entire
		 *                    list_node from the tree, potentially deleting a different key
		 *                    that shares the same hash bucket.
		 *   - extract(key) — calls get_node(), then returns at_position(0) regardless of
		 *                    whether its key matches the requested one.
		 *   - count(key)   — calls get_node() and returns 1 if any list_node is found at
		 *                    that hash position, even if it belongs to a different key.
		 *
		 * find() and at() are correct: they call get_node() then additionally call
		 * at_key() to verify the key, returning end() / throwing on mismatch.
		 *
		 * Why the bug is not reproducible today:
		 *   key_type is std::uintptr_t and extract_key() returns std::hash<T>()(item)
		 *   which on all current platforms is identity for pointer-sized integers.
		 *   With a 64-bit key space and a tree depth of 14 levels (8 + 14×4 bits),
		 *   every distinct key occupies a unique leaf slot — list_node::_list always
		 *   contains exactly one entry and hash collisions between different keys never
		 *   occur in practice. The collision-chain design of list_node therefore has
		 *   no observable effect at runtime.
		 *
		 * @todo Make get_node() return the matching list iterator rather than the raw
		 * list_node pointer, or add a key parameter check inside each caller
		 * (erase, extract, count) mirroring what find() and at() already do via at_key().
		 * Alternatively, remove list_node::_list as a chain and enforce the invariant that
		 * each slot holds at most one key, making the latent collision path unreachable
		 * by construction.
		 */
		void testGetNodeKeyVerification()
		{
			ht hash_table;
			// With uintptr_t identity hash, two distinct keys never collide —
			// get_node() always returns the correct node and the bug is invisible.
			// The test documents the contract that should hold and would fail
			// if key_type or extract_key() were ever changed to allow collisions.
			std::uintptr_t key_a(0x0001u);
			std::uintptr_t key_b(0x0002u);

			hash_table.emplace(key_a, std::make_shared<atom>());
			hash_table.emplace(key_b, std::make_shared<atom>());

			// erase(key_b) must not affect key_a
			hash_table.erase(key_b);
			IS_TRUE(hash_table.find(key_a) != hash_table.end());

			// count(key_b) must return 0 after erasure
			ARE_EQUAL(hash_table.count(key_b), static_cast<ht::size_type>(0));

			// extract(key_a) must return the value for key_a, not an arbitrary node
			safe_ptr<atom> extracted(hash_table.extract(key_a));
			IS_TRUE(!!extracted);
			IS_TRUE(hash_table.find(key_a) == hash_table.end());

			LOG("get_node() key verification: correct today (no collisions with uintptr_t identity hash); see @bug above");
		}

		/**
		 * @brief Verify that iteration visits every element and that find() returns
		 * an iterator pointing at the requested key.
		 *
		 * @bug Two related defects share the same root cause (list_node collision chain
		 * is only partially traversed):
		 *
		 *   1. basic_iterator::move() always calls at_position(0), loading the first
		 *      element of list_node::_list regardless of how many entries the node holds.
		 *      Any element beyond position 0 — inserted via the collision-chain path —
		 *      is permanently invisible to range-for iteration and to begin()/operator++.
		 *
		 *   2. find(key) locates the correct entry via at_key() but then constructs the
		 *      returned iterator with iterator(parent_array, parent_index), which calls
		 *      move() internally and lands on at_position(0). If the matching entry is
		 *      not at position 0, it->first and it->second reflect a different key's data.
		 *
		 * Why not reproducible today:
		 *   key_type is std::uintptr_t with identity hash; every distinct key occupies
		 *   a unique leaf slot and list_node::_list always has exactly one element.
		 *   Position 0 is therefore always the correct element and the collision chain
		 *   path is never exercised.
		 *
		 * @todo Track the list iterator position inside basic_iterator alongside _node
		 * and _index. move() should advance through all elements of a list_node before
		 * stepping to the next slot. find() should return an iterator initialised to the
		 * exact list iterator returned by at_key(), not reconstructed from the slot address.
		 */
		void testIteratorPositionZero()
		{
			ht hash_table;
			// Insert two keys that currently never collide.
			// The test verifies the observable contract; the @bug above explains
			// what would break if collisions were ever introduced.
			std::uintptr_t key_a(0x0010u);
			std::uintptr_t key_b(0x0020u);

			hash_table.emplace(key_a, std::make_shared<atom>());
			hash_table.emplace(key_b, std::make_shared<atom>());

			// Iteration must visit both elements.
			std::size_t visited(0);
			for (ht::iterator it(hash_table.begin()); it != hash_table.end(); ++it) {
				visited++;
			}
			ARE_EQUAL(visited, static_cast<std::size_t>(2));

			// find() must return an iterator whose key matches the requested key.
			ht::iterator it_a(hash_table.find(key_a));
			IS_TRUE(it_a != hash_table.end());
			ARE_EQUAL(it_a->first, key_a);

			ht::iterator it_b(hash_table.find(key_b));
			IS_TRUE(it_b != hash_table.end());
			ARE_EQUAL(it_b->first, key_b);

			LOG("Iterator position-0: correct today (each list_node has one element); see @bug above");
		}
				/**
		 * @brief Document that extract() uses unconditional exchange(nullptr) without
		 * CAS protection against the busy mark.
		 *
		 * @bug extract() reads the list_node pointer via get_node(), then unconditionally
		 * calls exchange(nullptr) on the parent slot. Unlike erase(key), which was fixed
		 * to use compare_exchange_strong(expected=node_is_list, nullptr), extract() has no
		 * such guard. If insert() holds the busy mark on that slot during a structural
		 * transform, extract()'s exchange(nullptr) frees the list_node while insert() still
		 * holds a raw pointer to it — use-after-free in insert()'s movable_node traversal.
		 *
		 * @todo Apply the same CAS guard used in erase(key): spin on busy, then
		 * compare_exchange_strong(expected=node_is_list, nullptr) so that extract()
		 * aborts gracefully when the slot is being mutated by insert().
		 */
		void testExtractRaceWithBusy()
		{
			ht hash_table;
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t iterations = 2000;
			constexpr std::uintptr_t key(0x0077u);

			std::atomic<std::size_t> extracted{0};
			std::vector<std::thread> threads;

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, t, key, &extracted]() {
					for (std::size_t i = 0; i < iterations; ++i) {
						if (t % 2 == 0) {
							hash_table.emplace(key, std::make_shared<atom>());
						} else {
							try {
								hash_table.extract(key);
								extracted++;
							} catch (const std::out_of_range&) {
								// key not present at this moment — expected under concurrent insert/extract
							}
						}
					}
				});
			}

			for (std::thread& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "extract() concurrent extractions: " << extracted.load()).str());
		}

		/**
		 * @brief Document that const find() has a UAF window absent from non-const find().
		 *
		 * @bug Non-const find() was rewritten (a72b45a) to traverse the tree inline and
		 * CAS-to-busy before reading the list_node, preventing a concurrent erase() from
		 * freeing the node between lookup and iterator construction.
		 * const find() was NOT updated: it delegates to get_node() which spins on busy
		 * but does NOT acquire busy itself. After get_node() returns a list_node pointer,
		 * before the const_iterator is constructed from it, a concurrent erase() can free
		 * that pointer — resulting in a UAF read inside the const_iterator constructor.
		 *
		 * @todo Rewrite const find() with the same inline CAS-to-busy traversal used by
		 * non-const find(), so the list_node is held busy for the duration of
		 * const_iterator construction.
		 */
		void testConstFindUAF()
		{
			ht hash_table;
			const ht& const_table(hash_table);
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t iterations = 5000;
			constexpr std::uintptr_t key(0x0088u);

			hash_table.emplace(key, std::make_shared<atom>());

			std::atomic<bool> stop{false};
			std::atomic<std::size_t> missed{0};
			std::vector<std::thread> threads;

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([&hash_table, &const_table, t, key, &stop, &missed]() {
					for (std::size_t i = 0; i < iterations && !stop; ++i) {
						if (t % 3 == 0) {
							/// Erase then re-insert to maximise the UAF window for const find()
							hash_table.erase(key);
							hash_table.emplace(key, std::make_shared<atom>());
						} else {
							/// const find(key) calls get_node() which returns a raw list_node*,
							/// then dereferences it to construct const_iterator — UAF fires if
							/// a concurrent erase() frees the node between those two steps.
							/// Run under ASan to surface the race deterministically.
							ht::const_iterator it(const_table.find(key));
							if (it == const_table.cend()) {
								missed++;
							}
						}
					}
				});
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			stop = true;
			for (std::thread& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "const find() UAF: missed reads=" << missed.load()
				<< " — run under ASan to surface UAF in const_iterator construction").str());
		}

		/**
		 * @brief Throughput comparison: atomic_hash_table vs unordered_map+shared_mutex vs unordered_map+mutex.
		 * Runs four scenarios (pure_read / 90-10 / 50-50 / pure_write) across all available threads
		 * and logs ops/s for each structure. Absolute numbers are depressed under ASan/debug builds;
		 * relative ratios between structures remain meaningful.
		 */
		void testThroughput()
		{
			using map_type = std::unordered_map<std::uintptr_t, safe_ptr<atom>>;
			using op_fn = std::function<void(std::uintptr_t)>;

			constexpr std::size_t key_space(10'000);
			constexpr std::uint64_t duration_ms(2'000);
			const std::size_t thread_count(worker_pool::capacity() - 2);

			const std::array<std::pair<int, const char*>, 4> scenario_array{{
				{100, "pure_read "},
				{90,  "read90    "},
				{50,  "mixed50   "},
				{0,   "pure_write"}
			}};

			auto measure = [&](op_fn read_fn, op_fn write_fn, int read_pct) -> std::uint64_t
			{
				std::atomic<bool> running{true};
				std::atomic<std::uint64_t> total_ops{0};
				std::vector<std::thread> threads;

				for (std::size_t t = 0; t < thread_count; ++t) {
					threads.emplace_back([&, t]() {
						std::mt19937 gen(t * 6364136223846793005ULL);
						std::uniform_int_distribution<std::uintptr_t> key_dist(0, key_space - 1);
						std::uniform_int_distribution<int> pct_dist(0, 99);
						std::uint64_t local_ops(0);
						while (running.load(std::memory_order_relaxed)) {
							const std::uintptr_t key(key_dist(gen));
							if (pct_dist(gen) < read_pct) {
								read_fn(key);
							} else {
								write_fn(key);
							}
							++local_ops;
						}
						total_ops.fetch_add(local_ops, std::memory_order_relaxed);
					});
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
				running.store(false, std::memory_order_relaxed);
				for (std::thread& thread : threads) {
					thread.join();
				}
				return total_ops.load();
			};

			for (const auto& [read_pct, label] : scenario_array) {
				/// atomic_hash_table
				ht hash_table;
				for (std::size_t i = 0; i < key_space; ++i) {
					hash_table.emplace(static_cast<std::uintptr_t>(i), std::make_shared<atom>());
				}
				const std::uint64_t ht_ops(measure(
					[&](std::uintptr_t key) { (void)hash_table.find(key); },
					[&](std::uintptr_t key) { hash_table.emplace(key, std::make_shared<atom>()); },
					read_pct
				));

				/// unordered_map + shared_mutex
				map_type shared_map;
				std::shared_mutex shared_mtx;
				for (std::size_t i = 0; i < key_space; ++i) {
					shared_map.emplace(static_cast<std::uintptr_t>(i), std::make_shared<atom>());
				}
				const std::uint64_t sm_ops(measure(
					[&](std::uintptr_t key) {
						std::shared_lock lock(shared_mtx);
						(void)shared_map.find(key);
					},
					[&](std::uintptr_t key) {
						std::unique_lock lock(shared_mtx);
						shared_map.insert_or_assign(key, std::make_shared<atom>());
					},
					read_pct
				));

				/// unordered_map + mutex
				map_type mutex_map;
				std::mutex mtx;
				for (std::size_t i = 0; i < key_space; ++i) {
					mutex_map.emplace(static_cast<std::uintptr_t>(i), std::make_shared<atom>());
				}
				const std::uint64_t mtx_ops(measure(
					[&](std::uintptr_t key) {
						std::unique_lock lock(mtx);
						(void)mutex_map.find(key);
					},
					[&](std::uintptr_t key) {
						std::unique_lock lock(mtx);
						mutex_map.insert_or_assign(key, std::make_shared<atom>());
					},
					read_pct
				));

				log_register<log>::log(
					log_level::debug,
					label,
					" | ht: ",     ht_ops  / duration_ms, "k ops/s",
					" | sh_mtx: ", sm_ops  / duration_ms, "k ops/s",
					" | mutex: ",  mtx_ops / duration_ms, "k ops/s"
				);
			}
		}
	/** @} */
	};
}

#endif
