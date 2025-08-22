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
