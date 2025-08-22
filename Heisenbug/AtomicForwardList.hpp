/**
 @file AtomicForwardList.hpp
 @date 15.04.21
 @copyright Cadabra Systems Inc
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_AtomicForwardList_hpp
#define Chaos_Heisenbug_AtomicForwardList_hpp

#include "../Heisenbug.hpp"

#include "../AtomicForwardList.hpp"
#include "../SafePointer.hpp"
#include "../WorkerPool.hpp"

#include <thread>
#include <random>
#include <vector>
#include <queue>
#include <map>

namespace chaos {
	class atomic_forward_list_test : public heisen_test
	{
	/** @name Classes */
	/** @{ */
	public:
		struct atom
		{
			atom(int underscore = 0)
			:
				_(underscore)
			{

			}
			int _;
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		atomic_forward_list_test()
		:
			heisen_test("AtomicForwardList")
		{

		};
		virtual ~atomic_forward_list_test() = default;
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
		atomic_forward_list<safe_ptr<atom>> _forward_list;
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
		 * 2) случайно, массов, долго-долго атаковать с этих потоков один и тот же atomic_forward_list
		 * 2а) добавляя
		 * 2б) удаляляя
		 * 2в) итерируя по коллекции
		 */
		void testStress()
		{
			constexpr std::uint64_t queue_op_count(10'000);
			constexpr std::uint64_t list_default_size(queue_op_count * 0.1);

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
				for (; tt < list_default_size; ++tt) {
					op_vector.back().emplace(op::insert);
				}
				for (; tt < queue_op_count; ++tt) {
					op_vector.back().emplace(static_cast<op>(int_distribution(generator)));
				}
			}

			std::function<void(atomic_forward_list<safe_ptr<atom>>&, std::queue<op>&)> thread_function
			(
				[&list_default_size](atomic_forward_list<safe_ptr<atom>>& forward_list, std::queue<op>& op_queue)
				{
					atomic_forward_list<safe_ptr<atom>>::const_iterator e(forward_list.cend());
					try {
						std::uint64_t o(0);
						while (!op_queue.empty()) {
							if (o++ == list_default_size) {
								log_register<log>::log(log_level::debug, list_default_size, " operations are passed on ", std::this_thread::get_id(), " thread");
								o = 0;
							}
							switch (op_queue.front()) {
								case op::iterate: {
									for (atomic_forward_list<safe_ptr<atom>>::iterator h = forward_list.begin(); h != forward_list.end(); ++h) {
										(*h)->_ += 1;
									}
								} break;

								case op::insert:
									if (forward_list.emplace(e, 0) == e) {
										return ;
									}
									break;

								case op::clear:
									forward_list.clear();
									break;

								case op::pop_front: {
									atomic_forward_list<safe_ptr<atom>>::const_iterator b(forward_list.cbegin());
									if (e != b) {
										forward_list.erase(b);
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
				thread_vector.emplace_back(thread_function, std::ref(_forward_list), std::ref(op_vector.at(t)));
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
