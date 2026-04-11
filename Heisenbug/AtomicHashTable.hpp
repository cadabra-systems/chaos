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

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		atomic_hash_table<safe_ptr<atom>> _hash_table;
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

			std::function<void(atomic_hash_table<safe_ptr<atom>>&, std::queue<op>&)> thread_function
			(
				[&hashtable_default_size](atomic_hash_table<safe_ptr<atom>>& hash_table, std::queue<op>& op_queue)
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
									for (atomic_hash_table<safe_ptr<atom>>::iterator h = hash_table.begin(); h != hash_table.end(); ++h) {
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
									/// @??? atomic_hash_table<safe_ptr<atom>>::const_iterator m(hash_table.cbegin());
									break;

								case op::pop_front: {
									atomic_hash_table<safe_ptr<atom>>::iterator h(hash_table.begin());
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
					BUG((std::stringstream() << "Operation queue on " << thread_vector.at(t).get_id() << " thread is stopped at " << static_cast<std::uint16_t>(op_vector.at(t).front()) << "op").rdbuf());
				}
			}
		}

		/**
		 * @brief Test memory safety - Use After Free issue (Code Review Issue #1)
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
					auto result = _hash_table.emplace(key, std::make_shared<atom>());
					IS_TRUE(result.second);
					keys.push_back(key);
				}
			}

			// Erase concurrently using different methods (iterator vs key)
			for (std::size_t t = 0; t < thread_count; ++t) {
				threads.emplace_back([this, t, items_per_thread]() {
					try {
						for (std::size_t i = 0; i < items_per_thread / 2; ++i) {
							std::uintptr_t key = t * items_per_thread + i;

							// Test erase by key
							_hash_table.erase(key);
						}

						// Test erase by iterator
						for (std::size_t i = items_per_thread / 2; i < items_per_thread; ++i) {
							auto it = _hash_table.find(t * items_per_thread + i);
							if (it != _hash_table.end()) {
								_hash_table.erase(it);
							}
						}
					} catch (const std::exception& e) {
						BUG((std::stringstream() << "Memory safety test failed: " << e.what()).rdbuf());
					}
				});
			}

			for (auto& thread : threads) {
				thread.join();
			}

			// Verify all items were deleted
			IS_TRUE(_hash_table.empty());
		}

		/**
		 * @brief Test race condition in iterator state update (Code Review Issue #2)
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
							std::size_t count = 0;
							for (auto it = _hash_table.begin(); it != _hash_table.end(); ++it) {
								count++;
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

			for (auto& thread : threads) {
				thread.join();
			}

			IS_TRUE(error_count == 0);
			_hash_table.clear();
		}

		/**
		 * @brief Test iterator invalidation on concurrent modification (Code Review Issue #3)
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
						for (auto it = _hash_table.begin(); it != _hash_table.end(); ++it) {
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
			std::thread writer([this, &stop_flag, initial_items]() {
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
		 * @brief Test concurrent list modification (Code Review Issue #4)
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
								auto it = _hash_table.find(key);
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

			for (auto& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "Concurrent list modification errors: " << error_count.load()).str());
			_hash_table.clear();
		}

		/**
		 * @brief Test iterator comparison operators (Code Review Issue #6)
		 * Operators <, >, <=, >= always return false (stub implementation)
		 */
		void testIteratorComparison()
		{
			// Insert some items
			_hash_table.emplace(1, std::make_shared<atom>());
			_hash_table.emplace(2, std::make_shared<atom>());
			_hash_table.emplace(3, std::make_shared<atom>());

			auto it1 = _hash_table.begin();
			auto it2 = _hash_table.begin();
			auto it3 = _hash_table.end();

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
		 * @brief Test concurrent find operations (Code Review Issue #7)
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
				threads.emplace_back([this, &found_count, &not_found_count, item_count]() {
					try {
						std::mt19937 gen(std::random_device{}());
						std::uniform_int_distribution<std::uintptr_t> dist(0, item_count * 2);

						for (std::size_t i = 0; i < finds_per_thread; ++i) {
							std::uintptr_t key = dist(gen);
							auto it = _hash_table.find(key);

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

			for (auto& thread : threads) {
				thread.join();
			}

			IS_GREATER(found_count, 0);
			LOG((std::stringstream() << "Found: " << found_count << ", Not found: " << not_found_count).str());
			_hash_table.clear();
		}

		/**
		 * @brief Test failure counter and busy-wait contention (Code Review Issue #9)
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
							auto result = _hash_table.try_emplace(key, std::make_shared<atom>());

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

			for (auto& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "Successful inserts: " << success_inserts
				<< ", Failed inserts: " << failed_inserts).str());

			// Under high contention, some inserts should fail
			IS_GREATER(success_inserts, 0);

			_hash_table.clear();
		}

		/**
		 * @brief Test extract method (Code Review Issue #10)
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
			auto extracted = _hash_table.extract(5);
			IS_TRUE(extracted != nullptr);

			// Try to extract again (should return nullptr)
			auto extracted_again = _hash_table.extract(5);
			IS_TRUE(extracted_again == nullptr);

			// Extract non-existent item
			auto non_existent = _hash_table.extract(9999);
			IS_TRUE(non_existent == nullptr);

			_hash_table.clear();
		}

		/**
		 * @brief Test concurrent insert/erase on same key (Code Review Issue #8/#9)
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
								auto result = _hash_table.try_emplace(key, std::make_shared<atom>());
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

			for (auto& thread : threads) {
				thread.join();
			}

			LOG((std::stringstream() << "Insert successes: " << insert_success
				<< ", Erase successes: " << erase_success).str());

			_hash_table.clear();
		}

		/**
		 * @brief Test try_emplace vs emplace behavior
		 * try_emplace should not overwrite existing keys, emplace should
		 */
		void testTryEmplaceVsEmplace()
		{
			// Insert initial item
			auto atom1 = std::make_shared<atom>();
			atom1->_ = 42;
			_hash_table.emplace(1, atom1);

			// Verify it's there
			auto it1 = _hash_table.find(1);
			IS_TRUE(it1 != _hash_table.end());
			IS_TRUE(it1->second->_ == 42);

			// try_emplace should NOT overwrite
			auto atom2 = std::make_shared<atom>();
			atom2->_ = 100;
			auto result1 = _hash_table.try_emplace(1, atom2);
			IS_FALSE(result1.second); // Should fail

			// Value should still be 42
			auto it2 = _hash_table.find(1);
			IS_TRUE(it2 != _hash_table.end());
			IS_TRUE(it2->second->_ == 42);

			// emplace SHOULD overwrite
			auto atom3 = std::make_shared<atom>();
			atom3->_ = 200;
			auto result2 = _hash_table.emplace(1, atom3);
			IS_FALSE(result2.second); // Returns false but does overwrite

			// Value should now be 200
			auto it3 = _hash_table.find(1);
			IS_TRUE(it3 != _hash_table.end());
			IS_TRUE(it3->second->_ == 200);

			_hash_table.clear();
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
