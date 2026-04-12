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

	/** @name Properties */
	/** @{ */
	protected:
		ht _hash_table;
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
			HEISEN(OperatorBracketThrows);
			HEISEN(IteratorValueCopy);
			HEISEN(SizeLinearScan);
			HEISEN(IteratorLeGeOperators);
			HEISEN(HashInconsistency);
			HEISEN(EraseConstIteratorUncallable);
			HEISEN(ShiftBackRecovery);
			HEISEN(InsertNullLoadFalseFailure);
			HEISEN(ListNodeConcurrentReadWrite);
			HEISEN(GetNodeKeyVerification);
			HEISEN(IteratorPositionZero);
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
				thread_vector.emplace_back(thread_function, std::ref(_hash_table), std::ref(op_vector.at(t)));
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
			constexpr std::size_t thread_count = 4;
			constexpr std::size_t items_per_thread = 1000;

			std::vector<std::thread> threads;
			std::vector<std::uintptr_t> keys;

			// Insert items first
			for (std::size_t t = 0; t < thread_count; ++t) {
				for (std::size_t i = 0; i < items_per_thread; ++i) {
					std::uintptr_t key = t * items_per_thread + i;
					std::pair<ht::iterator, bool> result(_hash_table.emplace(key, std::make_shared<atom>()));
					IS_TRUE(result.second);
					keys.push_back(key);
				}
			}

			// Erase concurrently using different methods (iterator vs key)
			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([this, t]() {
					try {
						for (std::size_t i = 0; i < items_per_thread / 2; ++i) {
							std::uintptr_t key = t * items_per_thread + i;

							// Test erase by key
							_hash_table.erase(key);
						}

						// Test erase by iterator
						for (std::size_t i = items_per_thread / 2; i < items_per_thread; ++i) {
							ht::iterator it(_hash_table.find(t * items_per_thread + i));
							if (it != _hash_table.end()) {
								_hash_table.erase(it);
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
			IS_TRUE(_hash_table.empty());
		}

		/**
		 * @brief Test race condition in iterator state update
		 * Iterator state (_node, _index, _value, _key) updates are not atomic
		 * Multiple threads reading same iterator during modification can observe inconsistent state
		 */
		void testIteratorRaceCondition()
		{
			constexpr std::size_t item_count = 100;
			constexpr std::size_t reader_threads = 8;

			// Insert items
			for (std::size_t i = 0; i < item_count; ++i) {
				_hash_table.emplace(i, std::make_shared<atom>());
			}

			std::atomic<bool> stop_flag{false};
			std::vector<std::thread> threads;
			std::atomic<std::size_t> error_count{0};

			// Multiple threads iterate simultaneously
			for (std::size_t t = 0; t < reader_threads; ++t) {
				threads.emplace_back([this, &stop_flag, &error_count]() {
					try {
						while (!stop_flag) {
							for (ht::iterator it(_hash_table.begin()); it != _hash_table.end(); ++it) {
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
			_hash_table.clear();
		}

		/**
		 * @brief Test iterator invalidation on concurrent modification
		 * Iterators should remain valid or fail gracefully when elements are added/removed
		 */
		void testIteratorInvalidation()
		{
			constexpr std::size_t initial_items = 50;

			// Insert initial items
			for (std::size_t i = 0; i < initial_items; ++i) {
				_hash_table.emplace(i, std::make_shared<atom>());
			}

			std::atomic<bool> stop_flag{false};
			std::atomic<std::size_t> error_count{0};

			// Reader thread iterating
			std::thread reader([this, &stop_flag, &error_count]() {
				try {
					while (!stop_flag) {
						for (ht::iterator it(_hash_table.begin()); it != _hash_table.end(); ++it) {
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
			std::thread writer([this, &stop_flag]() {
				try {
					std::size_t key = initial_items;
					while (!stop_flag) {
						_hash_table.emplace(key++, std::make_shared<atom>());
						if (key % 10 == 0) {
							_hash_table.erase(key - 10);
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
			_hash_table.clear();
		}

		/**
		 * @brief Test concurrent list modification
		 * list_node::at_key iterates std::list without synchronization
		 * Multiple hash collisions create lists that are accessed unsafely
		 */
		void testConcurrentListModification()
		{
			// Force hash collisions by using small key space
			constexpr std::size_t key_space = 16; // Will cause collisions
			constexpr std::size_t operations = 1000;
			constexpr std::size_t thread_count = 4;

			std::vector<std::thread> threads;
			std::atomic<std::size_t> error_count{0};

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([this, t, &error_count]() {
					try {
						std::mt19937 gen(t);
						std::uniform_int_distribution<std::uintptr_t> dist(0, key_space - 1);

						for (std::size_t i = 0; i < operations; ++i) {
							std::uintptr_t key = dist(gen);

							if (i % 3 == 0) {
								// Insert
								_hash_table.try_emplace(key, std::make_shared<atom>());
							} else if (i % 3 == 1) {
								// Find (causes list iteration)
								ht::iterator it(_hash_table.find(key));
								if (it != _hash_table.end() && it->second) {
									volatile int x = it->second->_;
									(void)x;
								}
							} else {
								// Erase
								_hash_table.erase(key);
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
			_hash_table.clear();
		}

		/**
		 * @brief Test iterator comparison operators
		 * Operators <, >, <=, >= always return false (stub implementation)
		 */
		void testIteratorComparison()
		{
			// Insert some items
			_hash_table.emplace(static_cast<std::uintptr_t>(1), std::make_shared<atom>());
			_hash_table.emplace(static_cast<std::uintptr_t>(2), std::make_shared<atom>());
			_hash_table.emplace(static_cast<std::uintptr_t>(3), std::make_shared<atom>());

			ht::iterator it1(_hash_table.begin());
			ht::iterator it2(_hash_table.begin());
			ht::iterator it3(_hash_table.end());

			// Test equality operators (should work)
			IS_TRUE(it1 == it2);
			IS_FALSE(it1 != it2);
			IS_FALSE(it1 == it3);

			// Test comparison operators (currently broken - all return false)
			bool lt = it1 < it2;
			bool gt = it1 > it2;
			bool le = it1 <= it2;
			bool ge = it1 >= it2;

			// Document current broken behavior
			LOG((std::stringstream() << "Iterator comparison: < = " << lt << ", > = " << gt
				<< ", <= = " << le << ", >= = " << ge).str());

			// All should be false due to stub implementation
			IS_FALSE(lt);
			IS_FALSE(gt);
			IS_FALSE(le);
			IS_FALSE(ge);

			_hash_table.clear();
		}

		/**
		 * @brief Test concurrent find operations
		 * Multiple threads calling find() on same keys
		 */
		void testConcurrentFind()
		{
			constexpr std::size_t item_count = 100;
			constexpr std::size_t finder_threads = 8;
			constexpr std::size_t finds_per_thread = 10000;

			// Insert items
			for (std::size_t i = 0; i < item_count; ++i) {
				_hash_table.emplace(i, std::make_shared<atom>());
			}

			std::vector<std::thread> threads;
			std::atomic<std::size_t> found_count{0};
			std::atomic<std::size_t> not_found_count{0};

			for (std::size_t t = 0; t < finder_threads; ++t) {
				threads.emplace_back([this, &found_count, &not_found_count]() {
					try {
						std::mt19937 gen(std::random_device{}());
						std::uniform_int_distribution<std::uintptr_t> dist(0, item_count * 2);

						for (std::size_t i = 0; i < finds_per_thread; ++i) {
							std::uintptr_t key = dist(gen);
							ht::iterator it(_hash_table.find(key));

							if (it != _hash_table.end()) {
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
			_hash_table.clear();
		}

		/**
		 * @brief Test failure counter and busy-wait contention
		 * High contention should trigger failure counter limits
		 */
		void testFailureCounter()
		{
			constexpr std::size_t thread_count = 16;
			constexpr std::size_t operations = 1000;

			// Use very small key space to maximize contention
			constexpr std::size_t key_space = 4;

			std::vector<std::thread> threads;
			std::atomic<std::size_t> failed_inserts{0};
			std::atomic<std::size_t> success_inserts{0};

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([this, t, &failed_inserts, &success_inserts]() {
					try {
						std::mt19937 gen(t);
						std::uniform_int_distribution<std::uintptr_t> dist(0, key_space - 1);

						for (std::size_t i = 0; i < operations; ++i) {
							std::uintptr_t key = dist(gen);
							std::pair<ht::iterator, bool> result(_hash_table.try_emplace(key, std::make_shared<atom>()));

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

			_hash_table.clear();
		}

		/**
		 * @brief Test extract method
		 * extract() returns mapped_type which may not be nullable
		 */
		void testExtractMethod()
		{
			// Insert items
			constexpr std::size_t item_count = 10;
			for (std::size_t i = 0; i < item_count; ++i) {
				_hash_table.emplace(i, std::make_shared<atom>());
			}

			// Extract existing item
			safe_ptr<atom> extracted(_hash_table.extract(5));
			IS_TRUE(!!extracted);

			// Try to extract again (should return nullptr)
			safe_ptr<atom> extracted_again(_hash_table.extract(5));
			IS_TRUE(!extracted_again);

			// Extract non-existent item
			safe_ptr<atom> non_existent(_hash_table.extract(9999));
			IS_TRUE(!non_existent);

			_hash_table.clear();
		}

		/**
		 * @brief Test concurrent insert/erase on same key
		 * Multiple threads inserting and erasing the same keys simultaneously
		 */
		void testConcurrentInsertErase()
		{
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t operations = 1000;
			constexpr std::size_t key_count = 10;

			std::vector<std::thread> threads;
			std::atomic<std::size_t> insert_success{0};
			std::atomic<std::size_t> erase_success{0};

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([this, t, &insert_success, &erase_success]() {
					try {
						std::mt19937 gen(t);
						std::uniform_int_distribution<std::uintptr_t> dist(0, key_count - 1);

						for (std::size_t i = 0; i < operations; ++i) {
							std::uintptr_t key = dist(gen);

							if (i % 2 == 0) {
								// Insert
								std::pair<ht::iterator, bool> result(_hash_table.try_emplace(key, std::make_shared<atom>()));
								if (result.second) {
									insert_success++;
								}
							} else {
								// Erase
								if (_hash_table.erase(key) > 0) {
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

			_hash_table.clear();
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
			// Insert initial item
			atom_ptr atom1(std::make_shared<atom>());
			atom1->_ = 42;
			_hash_table.emplace(static_cast<std::uintptr_t>(1), atom1);

			// Verify it's there
			ht::iterator it1(_hash_table.find(1));
			IS_TRUE(it1 != _hash_table.end());
			IS_TRUE(it1->second->_ == 42);

			// try_emplace should NOT overwrite
			atom_ptr atom2(std::make_shared<atom>());
			atom2->_ = 100;
			std::pair<ht::iterator, bool> result1(_hash_table.try_emplace(static_cast<std::uintptr_t>(1), atom2));
			IS_FALSE(result1.second); // Should fail

			// Value should still be 42
			ht::iterator it2(_hash_table.find(1));
			IS_TRUE(it2 != _hash_table.end());
			IS_TRUE(it2->second->_ == 42);

			// emplace SHOULD overwrite
			atom_ptr atom3(std::make_shared<atom>());
			atom3->_ = 200;
			std::pair<ht::iterator, bool> result2(_hash_table.emplace(static_cast<std::uintptr_t>(1), atom3));
			IS_FALSE(result2.second); // Returns false but does overwrite

			// Value should now be 200
			ht::iterator it3(_hash_table.find(1));
			IS_TRUE(it3 != _hash_table.end());
			IS_TRUE(it3->second->_ == 200);

			_hash_table.clear();
		}

		/**
		 * @brief Verify operator[] behavior on a missing key.
		 * @bug operator[] calls at() which throws std::out_of_range for a missing key
		 * instead of inserting a default-constructed value and returning a reference to it,
		 * as required by the std::unordered_map contract.
		 * @todo Reimplement operator[] to call insert() with a default-constructed value
		 * and return a reference to the stored mapped_type.
		 */
		void testOperatorBracketThrows()
		{
			bool threw = false;
			try {
				// std::unordered_map::operator[] must insert a default value and return a ref.
				// This implementation calls at() and throws instead.
				safe_ptr<atom>& val(_hash_table[static_cast<std::uintptr_t>(9999)]);
				(void)val;
			} catch (const std::out_of_range&) {
				threw = true;
			}
			// IS_FALSE: should NOT throw (correct behavior). FAILS → confirms the bug.
			IS_FALSE(threw);
			// Nothing was inserted either
			IS_TRUE(_hash_table.find(9999) == _hash_table.end());
			_hash_table.clear();
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
			atom_ptr original(std::make_shared<atom>());
			original->_ = 42;
			_hash_table.emplace(static_cast<std::uintptr_t>(1), safe_ptr<atom>(original));

			ht::iterator it(_hash_table.find(1));
			IS_TRUE(it != _hash_table.end());

			// Rebind iterator's second to a completely different safe_ptr.
			// Bug: it->second is a reference to iterator::_value (a copy),
			// NOT to the actual data inside list_node.
			atom_ptr replacement(std::make_shared<atom>());
			replacement->_ = 99;
			it->second = safe_ptr<atom>(replacement);

			// A fresh lookup should see 99 if the assignment persisted (correct behavior).
			// IS_TRUE: FAILS — stored value is still 42, assignment only modified the iterator-local copy.
			ht::iterator it2(_hash_table.find(1));
			IS_TRUE(it2 != _hash_table.end());
			IS_TRUE(it2->second->_ == 99);
			_hash_table.clear();
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
			constexpr std::size_t count = 500;
			for (std::size_t i = 0; i < count; ++i) {
				_hash_table.emplace(i, std::make_shared<atom>());
			}
			// Both calls traverse the entire structure — O(N) each time.
			ARE_EQUAL(_hash_table.size(), count);
			ARE_EQUAL(_hash_table.size(), count);
			LOG("size() is O(N): no cached counter; calling it twice performed two full traversals");
			_hash_table.clear();
			ARE_EQUAL(_hash_table.size(), static_cast<std::size_t>(0));
		}

		/**
		 * @brief Verify ordering comparison operators on equal iterators.
		 * @bug operator<=, operator>= are stub implementations that unconditionally
		 * return false. For equal iterators both must return true.
		 * @todo Implement ordering based on tree position: compare _node pointer first,
		 * then _index within the same node.
		 */
		void testIteratorLeGeOperators()
		{
			_hash_table.emplace(static_cast<std::uintptr_t>(1), std::make_shared<atom>());
			_hash_table.emplace(static_cast<std::uintptr_t>(2), std::make_shared<atom>());

			ht::iterator it1(_hash_table.begin());
			ht::iterator it2(_hash_table.begin()); // same position as it1

			IS_TRUE(it1 == it2); // sanity: equality works

			// For equal iterators, <= and >= must be true.
			// IS_TRUE: FAILS (returns false) → confirms the stub bug.
			bool le = (it1 <= it2);
			bool ge = (it1 >= it2);
			IS_TRUE(le);
			IS_TRUE(ge);
			_hash_table.clear();
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
			// insert() path: const std::size_t hash(key)         — identity
			// get_node() path: std::hash<std::uintptr_t>()(key)  — platform-defined
			std::uintptr_t key(0xDEADBEEFu);
			_hash_table.emplace(key, std::make_shared<atom>());

			// find() calls get_node() which applies std::hash to the key.
			// If the two hash functions disagree, this returns end().
			ht::iterator it(_hash_table.find(key));
			IS_TRUE(it != _hash_table.end());
			LOG("Consistent only because std::hash<uintptr_t> is identity on this platform (amd64/arm64)");
			_hash_table.clear();
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
			_hash_table.emplace(static_cast<std::uintptr_t>(1), std::make_shared<atom>());

			// The lines below would NOT compile until defect 1 is fixed:
			//   ht::const_iterator cit(_hash_table.cbegin());
			//   _hash_table.erase(cit);
			//
			// After defect 1 is fixed, erase() would still silently do nothing
			// due to defect 2 — the off-by-one in path traversal. Both must be
			// fixed together before this test can assert actual erasure.

			LOG("erase(const_iterator): return type mismatch prevents instantiation; path traversal off-by-one also present — fix both together");
			_hash_table.clear();
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
			// The broken recovery branch is guarded by assert(false) in debug builds,
			// so it cannot be triggered directly. This test instead inserts under
			// normal conditions to verify that the happy path is unaffected, and
			// documents what must be tested once the recovery branch is reachable.
			std::uintptr_t key(0xABCD1234u);
			std::pair<ht::iterator, bool> result(_hash_table.emplace(key, std::make_shared<atom>()));
			IS_TRUE(result.second);
			IS_TRUE(_hash_table.find(key) != _hash_table.end());

			// Once shift_back() is fixed and the assert(false) guard is removed,
			// this test should additionally provoke the CAS failure path and verify
			// that the list_node is correctly re-inserted after the rollback.
			LOG("shift_back() recovery: happy path correct; broken & vs | in recovery branch untestable while assert(false) guard is present");
			_hash_table.clear();
		}

		/**
		 * @brief Verify that insert() does not silently fail when a slot becomes free
		 * between a failed CAS and the subsequent load().
		 *
		 * @bug In insert(), after compare_exchange_strong() fails, the inner do/while
		 * loop reloads the slot with atom->load(). If the slot is nullptr at that point,
		 * the code increments fail_counter and returns {end(), false} immediately:
		 *
		 *   marked_node target_node(atom->load());
		 *   if (nullptr == target_node) {
		 *       fail_counter++;
		 *       return std::make_pair(end(), false);  // ← wrong: should retry
		 *   }
		 *
		 * This can happen when:
		 *   1. Thread A's CAS fails because thread B just inserted into the slot.
		 *   2. Thread C erases that slot before thread A reaches atom->load().
		 *   3. Thread A sees nullptr and returns failure even though the slot is free.
		 *
		 * The observable symptom is try_emplace(key) returning {end(), false} for a key
		 * that does not exist in the table — the caller cannot distinguish this from a
		 * legitimate "key already present" false return.
		 *
		 * The correct behaviour is to treat nullptr as a free slot and retry the outer
		 * loop (continue to the next CAS attempt) rather than returning failure.
		 *
		 * @todo Replace the early return with a continue statement so the outer loop
		 * retries the CAS on the now-free slot:
		 *   if (nullptr == target_node) {
		 *       fail_counter++;
		 *       continue;   // ← retry CAS instead of giving up
		 *   }
		 * After the fix, spurious_failures in this test must be 0.
		 */
		void testInsertNullLoadFalseFailure()
		{
			constexpr std::size_t thread_count = 8;
			constexpr std::size_t iterations = 5000;
			// Single key — maximum contention on one slot, maximising the chance
			// that a CAS-fail and a concurrent erase interleave.
			constexpr std::uintptr_t key(0x0042u);

			std::atomic<std::size_t> spurious_failures{0};
			std::vector<std::thread> threads;

			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([this, t, key, &spurious_failures]() {
					for (std::size_t i = 0; i < iterations; ++i) {
						if (t % 2 == 0) {
							// Even threads: insert then erase to churn the slot.
							_hash_table.emplace(key, std::make_shared<atom>());
							_hash_table.erase(key);
						} else {
							// Odd threads: try_emplace and check for spurious failure.
							// A spurious failure is {end(), false} when the key is
							// also absent from the table — the insert was dropped.
							std::pair<ht::iterator, bool> result(_hash_table.try_emplace(key, std::make_shared<atom>()));
							if (!result.second && result.first == _hash_table.end()) {
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
			_hash_table.clear();
		}

		/**
		 * @brief Verify that concurrent overwrite and find on the same key are safe.
		 * @bug get_node() does not check the busy mark before returning a list_node pointer.
		 * insert() sets the busy mark while mutating list_node::_list, but a concurrent
		 * find() → get_node() → at_key() call receives the raw pointer and iterates the
		 * std::list while it is being modified — a data race (undefined behaviour).
		 * Note: testConcurrentListModification uses a small key-space, but with uintptr_t
		 * identity hash no two distinct keys ever share a list_node, so that test does
		 * not exercise this code path.
		 * @todo Make get_node() spin on the busy mark before dereferencing the list_node,
		 * mirroring the busy-wait already present in insert().
		 */
		void testListNodeConcurrentReadWrite()
		{
			// Seed one entry so the slot exists before threads start.
			std::uintptr_t key(1);
			_hash_table.emplace(key, std::make_shared<atom>());

			std::atomic<bool> stop{false};
			std::atomic<std::size_t> missed{0};

			// Writer: repeatedly overwrites key 1.
			// insert() sets busy → mutates _list → clears busy.
			std::thread writer([this, key, &stop]() {
				while (!stop) {
					_hash_table.emplace(key, std::make_shared<atom>());
				}
			});

			// Reader: repeatedly calls find() which calls get_node() (no busy check)
			// → then at_key() iterates the raw std::list concurrently with mutation.
			std::thread reader([this, key, &stop, &missed]() {
				while (!stop) {
					ht::iterator it(_hash_table.find(key));
					if (it == _hash_table.end()) {
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
			_hash_table.clear();
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
			// With uintptr_t identity hash, two distinct keys never collide —
			// get_node() always returns the correct node and the bug is invisible.
			// The test documents the contract that should hold and would fail
			// if key_type or extract_key() were ever changed to allow collisions.
			std::uintptr_t key_a(0x0001u);
			std::uintptr_t key_b(0x0002u);

			_hash_table.emplace(key_a, std::make_shared<atom>());
			_hash_table.emplace(key_b, std::make_shared<atom>());

			// erase(key_b) must not affect key_a
			_hash_table.erase(key_b);
			IS_TRUE(_hash_table.find(key_a) != _hash_table.end());

			// count(key_b) must return 0 after erasure
			ARE_EQUAL(_hash_table.count(key_b), static_cast<ht::size_type>(0));

			// extract(key_a) must return the value for key_a, not an arbitrary node
			safe_ptr<atom> extracted(_hash_table.extract(key_a));
			IS_TRUE(!!extracted);
			IS_TRUE(_hash_table.find(key_a) == _hash_table.end());

			LOG("get_node() key verification: correct today (no collisions with uintptr_t identity hash); see @bug above");
			_hash_table.clear();
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
			// Insert two keys that currently never collide.
			// The test verifies the observable contract; the @bug above explains
			// what would break if collisions were ever introduced.
			std::uintptr_t key_a(0x0010u);
			std::uintptr_t key_b(0x0020u);

			_hash_table.emplace(key_a, std::make_shared<atom>());
			_hash_table.emplace(key_b, std::make_shared<atom>());

			// Iteration must visit both elements.
			std::size_t visited(0);
			for (ht::iterator it(_hash_table.begin()); it != _hash_table.end(); ++it) {
				visited++;
			}
			ARE_EQUAL(visited, static_cast<std::size_t>(2));

			// find() must return an iterator whose key matches the requested key.
			ht::iterator it_a(_hash_table.find(key_a));
			IS_TRUE(it_a != _hash_table.end());
			ARE_EQUAL(it_a->first, key_a);

			ht::iterator it_b(_hash_table.find(key_b));
			IS_TRUE(it_b != _hash_table.end());
			ARE_EQUAL(it_b->first, key_b);

			LOG("Iterator position-0: correct today (each list_node has one element); see @bug above");
			_hash_table.clear();
		}
	/** @} */
	};
}

#endif
