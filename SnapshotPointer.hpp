/**
 @file SnapshotPointer.hpp
 @date 2026-05-08
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Cadabra_Chaos_SnapshotPointer_hpp
#define Cadabra_Chaos_SnapshotPointer_hpp

#include <memory>
#include <functional>

namespace chaos {
	/**
	 * @brief RCU-style smart pointer: wait-free reads, lock-free writes.
	 *
	 * Wraps std::shared_ptr<T> with atomic access semantics:
	 *   - load()   — returns an immutable snapshot, wait-free
	 *   - update() — copy-on-write via CAS loop, lock-free
	 *   - store()  — unconditional atomic swap
	 *
	 * Old snapshots remain valid (and live) as long as any caller holds
	 * the returned shared_ptr — reference counting is the grace period.
	 *
	 * Non-copyable and non-movable: the pointer must stay at a stable
	 * address so that concurrent atomic_load / compare_exchange operate
	 * on the same memory location.
	 */
	template<typename T>
	class snapshot_ptr
	{
	/** @name Aliases */
	/** @{ */
	public:
		using element_type = T;
		using snapshot_type = std::shared_ptr<const T>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		snapshot_ptr()
		:
			_value(std::make_shared<T>())
		{
		}

		explicit snapshot_ptr(std::shared_ptr<T> initial)
		:
			_value(std::move(initial))
		{
		}

		snapshot_ptr(const snapshot_ptr&) = delete;
		snapshot_ptr(snapshot_ptr&&) = delete;
		~snapshot_ptr() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		snapshot_ptr& operator=(const snapshot_ptr&) = delete;
		snapshot_ptr& operator=(snapshot_ptr&&) = delete;

		explicit operator bool() const noexcept
		{
			return !!std::atomic_load(&_value);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<T> _value;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		/**
		 * @brief Returns immutable snapshot of the current value. Wait-free.
		 * The snapshot remains valid for the lifetime of the returned shared_ptr,
		 * regardless of concurrent writes.
		 */
		snapshot_type load() const noexcept
		{
			return std::atomic_load(&_value);
		}

		/**
		 * @brief Applies transformer to the current value and atomically installs
		 * the result. Lock-free: retries if another thread writes concurrently.
		 * transformer may be called more than once under contention — keep it pure.
		 */
		void update(std::function<T(const T&)> transformer)
		{
			std::shared_ptr<T> old_value;
			std::shared_ptr<T> new_value;
			do {
				old_value = std::atomic_load(&_value);
				new_value = std::make_shared<T>(transformer(*old_value));
			} while (!std::atomic_compare_exchange_strong(&_value, &old_value, new_value));
		}

		/**
		 * @brief Unconditionally replaces the current value.
		 */
		void store(std::shared_ptr<T> value) noexcept
		{
			std::atomic_store(&_value, std::move(value));
		}
	/** @} */
	};
}

#endif
