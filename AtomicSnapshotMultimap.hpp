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
#include <set>
#include <cstdint>

namespace chaos {
	/**
	 * @brief Lock-free 1:N multimap with RCU-style snapshot reads.
	 *
	 * Maps K → {V...} using atomic_hash_table<K, shared_ptr<set<V, C>>>:
	 *   - Entry per key holds a shared_ptr<set> managed via table::exchange (CAS)
	 *   - load()   — find entry + return snapshot of the current set
	 *   - insert() — try_emplace fast path; otherwise CAS loop, copy-on-write with null pruning; idempotent
	 *   - remove() — CAS loop, copy-on-write removal with null pruning;
	 *                supports transparent lookup via C::is_transparent
	 *
	 * The K-keyed atomic_hash_table stores and compares K by value (std::hash<K>
	 * for navigation, K::operator== for disambiguation), so distinct keys with a
	 * colliding std::hash<K> never alias — no event/value misrouting on collision.
	 *
	 * Entries are permanent: once created for a key they are never removed,
	 * preventing ABA zombie-write races between remove and re-insert.
	 * Null values (!!v == false) are pruned lazily on each write.
	 *
	 * The set ordering and transparent-lookup key are defined by C.
	 * V must support operator bool(). C must be a strict weak order.
	 * K must satisfy std::hash<K> and K::operator==.
	 * For transparent remove(K, LookupKey), C::is_transparent must be defined
	 * and C must accept (V, LookupKey) and (LookupKey, V).
	 */
	template<typename K, typename V, typename C = std::less<V>>
	class atomic_snapshot_multimap
	{
	/** @name Aliases */
	/** @{ */
	public:
		using set = std::set<V, C>;

	private:
		using table_type = atomic_hash_table<K, std::shared_ptr<set>>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_snapshot_multimap() = default;
		~atomic_snapshot_multimap() = default;
		atomic_snapshot_multimap(const atomic_snapshot_multimap&) = delete;
		atomic_snapshot_multimap(atomic_snapshot_multimap&&) = delete;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		table_type _table;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		atomic_snapshot_multimap& operator=(const atomic_snapshot_multimap&) = delete;
		atomic_snapshot_multimap& operator=(atomic_snapshot_multimap&&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		/**
		 * @brief Inserts value under key, pruning null entries. Idempotent. Lock-free.
		 */
		void insert(const K& key, const V& value)
		{
			std::shared_ptr<set> initial(std::make_shared<set>(C{}));
			initial->insert(value);
			std::pair<typename table_type::iterator, bool> retval(_table.try_emplace(key, initial));
			if (retval.second) {
				return;
			}

			std::shared_ptr<set> expected(retval.first->second);
			std::shared_ptr<set> desired;
			do {
				desired = std::make_shared<set>(expected->key_comp());
				for (const V& item : *expected) {
					if (!!item) {
						desired->insert(item);
					}
				}
				desired->insert(value);
			} while (!_table.exchange(key, expected, desired));
		}

		/**
		 * @brief Removes the entry matching lookup_key under key, pruning null entries. Lock-free.
		 * Does nothing if the key has never been inserted or lookup_key is not found.
		 * Supports transparent lookup: LookupKey may differ from V if C::is_transparent
		 * is defined and C accepts (V, LookupKey) and (LookupKey, V).
		 */
		template<typename LookupKey>
		void remove(const K& key, const LookupKey& lookup_key)
		{
			typename table_type::iterator i(_table.find(key));
			if (i == _table.end()) {
				return;
			}
			std::shared_ptr<set> expected(i->second);
			std::shared_ptr<set> desired;
			do {
				desired = std::make_shared<set>(expected->key_comp());
				bool removed(false);
				const C& comp(expected->key_comp());
				for (const V& item : *expected) {
					if (!item) {
						continue;
					} else if (!removed && !comp(item, lookup_key) && !comp(lookup_key, item)) {
						removed = true;
						continue;
					}
					desired->insert(item);
				}
			} while (!_table.exchange(key, expected, desired));
		}

		/**
		 * @brief Returns an immutable snapshot of all values registered under key.
		 * Returns nullptr if the key has never been inserted.
		 * The snapshot remains valid for the lifetime of the returned shared_ptr
		 * regardless of concurrent inserts or removes.
		 */
		std::shared_ptr<const set> load(const K& key) const noexcept
		{
			typename table_type::const_iterator i(_table.find(key));
			return (i == _table.cend()) ? nullptr : i->second;
		}
	/** @} */
	};
}

#endif
