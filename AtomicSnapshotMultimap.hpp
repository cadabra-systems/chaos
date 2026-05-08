/**
 @file AtomicSnapshotMultimap.hpp
 @date 2026-05-08
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Cadabra_Chaos_AtomicSnapshotMultimap_hpp
#define Cadabra_Chaos_AtomicSnapshotMultimap_hpp

#include "AtomicHashTable.hpp"

#include <memory>
#include <vector>
#include <cstdint>

namespace chaos {
	/**
	 * @brief Lock-free 1:N multimap with RCU-style snapshot reads.
	 *
	 * Maps K → [V...] using atomic_hash_table<shared_ptr<vector<V>>>:
	 *   - Entry per key holds a shared_ptr<vector<V>> managed via CAS
	 *   - load()   — wait-free: find entry + atomic_load of vector snapshot
	 *   - insert() — lock-free: CAS loop, copy-on-write append with null pruning
	 *   - remove() — lock-free: CAS loop, copy-on-write removal with null pruning
	 *
	 * Entries are permanent: once created for a key they are never removed,
	 * preventing ABA zombie-write races between remove and re-insert.
	 * Null values (!!v == false) are pruned lazily on each write.
	 *
	 * V must support operator bool() and operator==().
	 */
	template<typename K, typename V>
	class atomic_snapshot_multimap
	{
	/** @name Aliases */
	/** @{ */
	public:
		using vector_type = std::vector<V>;

	private:
		using table_type = atomic_hash_table<std::shared_ptr<vector_type>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_snapshot_multimap() = default;
		~atomic_snapshot_multimap() = default;

		atomic_snapshot_multimap(const atomic_snapshot_multimap&) = delete;
		atomic_snapshot_multimap(atomic_snapshot_multimap&&) = delete;
		atomic_snapshot_multimap& operator=(const atomic_snapshot_multimap&) = delete;
		atomic_snapshot_multimap& operator=(atomic_snapshot_multimap&&) = delete;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		table_type _table;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		/**
		 * @brief Appends value under key, pruning null entries. Lock-free.
		 */
		void insert(const K& key, const V& value)
		{
			typename table_type::iterator it(fetch(static_cast<std::uintptr_t>(std::hash<K>()(key))));
			std::shared_ptr<vector_type> old_vector;
			std::shared_ptr<vector_type> new_vector;
			do {
				old_vector = std::atomic_load(&it->second);
				new_vector = std::make_shared<vector_type>();
				new_vector->reserve(old_vector->size() + 1);
				for (const V& item : *old_vector) {
					if (!!item) {
						new_vector->push_back(item);
					}
				}
				new_vector->push_back(value);
			} while (!std::atomic_compare_exchange_strong(&it->second, &old_vector, new_vector));
		}

		/**
		 * @brief Removes the first entry equal to value under key, pruning null entries. Lock-free.
		 * Does nothing if the key has never been inserted.
		 */
		void remove(const K& key, const V& value)
		{
			typename table_type::iterator it(_table.find(static_cast<std::uintptr_t>(std::hash<K>()(key))));
			if (it == _table.end()) {
				return;
			}
			std::shared_ptr<vector_type> old_vector;
			std::shared_ptr<vector_type> new_vector;
			do {
				old_vector = std::atomic_load(&it->second);
				new_vector = std::make_shared<vector_type>();
				new_vector->reserve(old_vector->size());
				bool removed(false);
				for (const V& item : *old_vector) {
					if (!item) {
						continue;
					} else if (!removed && item == value) {
						removed = true;
						continue;
					}
					new_vector->push_back(item);
				}
			} while (!std::atomic_compare_exchange_strong(&it->second, &old_vector, new_vector));
		}

		/**
		 * @brief Returns an immutable snapshot of all values registered under key.
		 * Returns nullptr if the key has never been inserted. Wait-free.
		 * The snapshot remains valid for the lifetime of the returned shared_ptr
		 * regardless of concurrent inserts or removes.
		 */
		std::shared_ptr<const vector_type> load(const K& key) const noexcept
		{
			auto it(_table.find(static_cast<std::uintptr_t>(std::hash<K>()(key))));
			if (it == _table.cend()) {
				return nullptr;
			}
			return std::atomic_load(&it->second);
		}

	private:
		/**
		 * @brief Finds or atomically creates the entry for table_key. Lock-free.
		 */
		typename table_type::iterator fetch(std::uintptr_t table_key)
		{
			do {
				{
					typename table_type::iterator it(_table.find(table_key));
					if (it != _table.end()) {
						return it;
					}
				}
				std::pair<typename table_type::iterator, bool> result(_table.try_emplace(table_key, std::make_shared<vector_type>()));
				if (result.second) {
					return result.first;
				}
			} while (true);
		}
	/** @} */
	};
}

#endif
