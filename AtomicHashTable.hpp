/**
 @file AtomicHashTable.hpp
 @date 27.07.2018
 @copyright Daniil A Megrabyan
 @author Daniil A Megrabyan
*/

#ifndef Cadabra_Chaos_AtomicHashTable_hpp
#define Cadabra_Chaos_AtomicHashTable_hpp

#include "MarkedPointer.hpp"

#include <atomic>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <iterator>

namespace chaos {
	/**
	 * @brief Concurrent Multi-level Arrays: Wait-free Extensible Hash Maps
	 * @url http://samos-conference.com/Resources_Samos_Websites/Proceedings_Repository_SAMOS/2013/Files/2013-IC-20.pdf
	 *
	 * @note [never-free design] list_node structs are never freed; only their std::list contents
	 * are cleared on erase(). This eliminates all UAF on list_node* without requiring
	 * epoch-based reclamation or hazard pointers. The node memory is bounded by the peak
	 * number of distinct keys ever inserted — acceptable for bounded workloads (e.g.,
	 * connection/session tracking with bounded concurrency).
	 *
	 * @note Template usage:
	 * atomic_hash_table<V> ≡ atomic_hash_table<std::uintptr_t, V>   (legacy form)
	 * atomic_hash_table<K, V> — K-keyed with std::hash<K> for tree navigation
	 *
	 * K must satisfy: std::hash<K> exists, K::operator== exists, K is copyable.
	 * Hash collisions on K (two distinct K with same std::hash<K> value) are resolved
	 * inside list_node by K::operator==.
	 */
	template <typename A, typename B = void>
	class atomic_hash_table
	{
	/** @name Aliases */
	/** @{ */
	public:
		using key_type = std::conditional_t<std::is_void_v<B>, std::uintptr_t, A>;
		using mapped_type = std::conditional_t<std::is_void_v<B>, A, B>;
		using hasher = std::hash<key_type>;
		using size_type = std::size_t;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const std::size_t hash_size = sizeof(std::size_t) * 8;
		static const std::size_t headkey_size = 8; /// @note ^2
		static const std::size_t slotkey_size = 4; /// @note ^2
		static const std::uint8_t node_is_list = 0b00;
		static const std::uint8_t node_is_busy = 0b01;
		static const std::uint8_t node_is_array = 0b10;

		/**
		 * @brief Derives the key from the value itself via std::hash<mapped_type>.
		 * @note Only meaningful for the legacy K=uintptr_t form. The corresponding
		 * no-key insert/emplace/try_emplace overloads are SFINAE-disabled when
		 * K is not std::uintptr_t — for custom K an explicit key is required.
		 */
		static std::uintptr_t extract_key(const mapped_type& item)
		{
			return std::hash<mapped_type>()(item);
		}

		static std::size_t calculate_head_key_size(std::size_t head_key_size, std::size_t slot_key_size)
		{
			std::size_t retval = (head_key_size < slot_key_size) ? slot_key_size : head_key_size;

			if (retval > atomic_hash_table::hash_size) {
				retval = atomic_hash_table::hash_size;
			}

			const std::size_t modulo((atomic_hash_table::hash_size - retval) & (slot_key_size - 1));

			return (modulo != 0) ? retval + modulo : retval;
		}
	/** @} */

	/** @name Classes */
	/** @{ */
	private:
		class array_node;
		class list_node;

		class abstract_node
		{
		friend atomic_hash_table;

		/** @name Constructors */
		/** @{ */
		protected:
			abstract_node(array_node* parent_node, std::size_t parent_index)
			:
				_parent_array(parent_node),
				_parent_index(parent_index)
			{

			}

		public:
			virtual ~abstract_node()
			{

			}
		/** @} */

		/** @name Properties */
		/** @{ */
		protected:
			array_node* _parent_array;
			std::size_t _parent_index;
		/** @} */

		/** @name Mutators */
		/** @{ */
		public:
			void set_parent(array_node* pointer, std::size_t index)
			{
				_parent_array = pointer;
				_parent_index = index;
			}
		/** @} */

		/** @name Getters */
		/** @{ */
		public:
			array_node* get_parent_array()
			{
				return _parent_array;
			}

			std::size_t get_parent_index() const
			{
				return _parent_index;
			}

			const array_node* get_parent_array() const
			{
				return _parent_array;
			}
		/** @} */
		};

		using marked_node = marked_ptr<abstract_node>;
		using atomic_marked_node = std::atomic<marked_node>;

		class list_node : public abstract_node
		{
		/** @name Classes */
		/** @{ */
		public:
			using data = std::pair<const key_type, mapped_type>;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			list_node(key_type key, std::size_t hash, mapped_type value, array_node* parent_node, std::size_t parent_index)
			:
				abstract_node(parent_node, parent_index),
				_list({data{key, value}}),
				_hash(hash)
			{

			}

			virtual ~list_node()
			{
				/// @todo run through _list marked_ptr::ptr
			}
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			std::list<data> _list;
			std::size_t _hash;
		/** @} */

		/** @name Getters */
		/** @{ */
		public:
			typename std::list<data>::const_iterator at_position(size_type index) const
			{
				return std::next(_list.cbegin(), index);
			}

			typename std::list<data>::iterator at_position(size_type index)
			{
				return std::next(_list.begin(), index);
			}

			typename std::list<data>::const_iterator at_key(key_type key) const
			{
				for (typename std::list<data>::const_reverse_iterator i = _list.crbegin(); i != _list.crend(); i++) { /// @xxx check threadsafe
					if (i->first == key) {
						return (++i).base();
					}
				}

				return _list.cend();
			}

			typename std::list<data>::iterator at_key(key_type key)
			{
				for (typename std::list<data>::reverse_iterator i = _list.rbegin(); i != _list.rend(); i++) { /// @xxx check threadsafe
					if (i->first == key) {
						return (++i).base();
					}
				}

				return _list.end();
			}

			size_type size() const
			{
				return _list.size();
			}

			/**
			 * @brief Remaining hash bits not yet consumed by the path from root to this node.
			 * Two keys sharing a bucket with equal hash_value() are indistinguishable
			 * by any deeper split — they must be separate-chained, never split apart.
			 */
			std::size_t hash_value() const
			{
				return _hash;
			}

			typename std::list<data>::iterator end()
			{
				return _list.end();
			}

			typename std::list<data>::const_iterator cend() const
			{
				return _list.cend();
			}
		/** @} */

		/** @name Mutators */
		/** @{ */
		public:
			void clear()
			{
				_list.clear();
			}

			/**
			 * @brief Removes the single entry matching key from this bucket (separate chaining).
			 * @return true if an entry was removed.
			 */
			bool erase(key_type key)
			{
				typename std::list<data>::iterator i(at_key(key));
				if (_list.end() == i) {
					return false;
				}
				_list.erase(i);
				return true;
			}

			/**
			 * @brief Removes the entry at the given position within this bucket.
			 * @return true if a position was removed.
			 */
			bool erase_at(size_type position)
			{
				if (position >= _list.size()) {
					return false;
				}
				_list.erase(at_position(position));
				return true;
			}

			std::size_t shift(std::size_t step, std::size_t mask)
			{
				std::size_t retval(_hash & mask);
				_hash >>= step;

				return retval;
			}

			std::size_t shift_back(std::size_t step, std::size_t index)
			{
				return (_hash = ((_hash << step) | index));
			}

			std::pair<typename std::list<data>::iterator, bool> emplace(key_type key, mapped_type value)
			{
				typename std::list<data>::iterator i(at_key(key));
				if (_list.end() != i) {
					i->second = value;
					return std::make_pair(i, false);
				}

				_list.emplace_back(key, value);
				return std::make_pair(std::prev(_list.end()), true);
			}

			std::pair<typename std::list<data>::iterator, bool> try_emplace(key_type key, mapped_type value)
			{
				typename std::list<data>::iterator i(at_key(key));
				if (_list.end() != i) {
					return std::make_pair(i, false);
				}

				_list.emplace_back(key, value);
				return std::make_pair(std::prev(_list.end()), true);
			}
		/** @} */
		};

		class array_node : public abstract_node
		{
		friend atomic_hash_table;

		/** @name Constructors */
		/** @{ */
		public:
			array_node(std::size_t length, array_node* parent_node, std::size_t parent_index)
			:
				abstract_node(parent_node, parent_index),
				_array(length)
			{

			}

			virtual ~array_node()
			{
				for (atomic_marked_node& atomic_node : _array) {
					marked_node raw_node(atomic_node.exchange(nullptr));
					if (raw_node.ptr()) {
						raw_node.dispose();
					}
				}
			}
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			std::vector<atomic_marked_node> _array;
		/** @} */

		/** @name Getters */
		/** @{ */
		public:
			atomic_marked_node& at(std::size_t index)
			{
				return _array.at(index);
			}

			const atomic_marked_node& at(std::size_t index) const
			{
				return _array.at(index);
			}

			std::size_t size() const
			{
				return _array.size();
			}
		/** @} */

		};


		union adapter
		{
			abstract_node* abstract;
			array_node* array;
			list_node* list;

			adapter(abstract_node* pointer)
			:
				abstract(pointer)
			{

			}
		};

	public:
		enum class iterator_direction : std::uint8_t
		{
			forward = 0x01,
			backward = 0x02
		};

		enum class iterator_end : std::uint8_t
		{
			middle = 0x00,
			first = 0x01,
			last = 0x02
		};

		template<bool IsConst = false>
		class basic_iterator
		{
		friend atomic_hash_table;

		/** @name Aliases */
		/** @{ */
		public:
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			using value_type = std::pair<const key_type&, mapped_type&>;
			using reference = typename std::conditional_t<IsConst, value_type const&, value_type&>;
			using pointer = typename std::conditional_t<IsConst, value_type const*, value_type*>;
			using node_pointer = array_node*;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			basic_iterator(node_pointer node, size_type index = 0)
			:
				_key(),
				_value(nullptr),
				_pair(_key, _value),
				_node(node),
				_index(index),
				_position(0)
			{
				move(iterator_direction::forward, false);
			}

			basic_iterator(node_pointer node, iterator_end end)
			:
				_key(),
				_value(nullptr),
				_pair(_key, _value),
				_node(node),
				_index(iterator_end::first == end ? 0 : _node->size()),
				_position(0)
			{
				if (iterator_end::first == end) {
					move(iterator_direction::forward, false);
				}
			}

			/**
			 * @brief Construct directly from a known list element while the node's busy mark
			 * is still held by the caller — safe to read data_it here because the slot
			 * is busy and erase() will spin until we release it.
			 * _position is the element's offset within its bucket, so iteration and
			 * iterator equality stay consistent for multi-entry (hash-collision) buckets.
			 */
			basic_iterator(list_node* list, typename std::list<typename list_node::data>::iterator data_it)
			:
				_key(data_it->first),
				_value(data_it->second),
				_pair(_key, _value),
				_node(list->get_parent_array()),
				_index(list->get_parent_index()),
				_position(static_cast<size_type>(std::distance(list->at_position(0), data_it)))
			{
			}

			basic_iterator(const basic_iterator<IsConst>& origin)
			:
				_key(origin._key),
				_value(origin._value),
				_pair(_key, _value),
				_node(origin._node),
				_index(origin._index),
				_position(origin._position)
			{
			}

			template <bool Const, std::enable_if_t<IsConst && Const != IsConst, int> = 0>
			basic_iterator(const basic_iterator<Const>& origin)
			:
				_key(origin._key),
				_value(origin._value),
				_pair(_key, _value),
				_node(origin._node),
				_index(origin._index),
				_position(origin._position)
			{
			}

			~basic_iterator()
			{
			}
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			key_type _key;
			mapped_type _value;
			value_type _pair;

			node_pointer _node;
			size_type _index;
			size_type _position; /// < offset within the bucket's list at (_node, _index)
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			/**
			 * @brief Reads the entry at `position` in the bucket at (node, index) under the
			 * busy mark, copying it into _key/_value when position < bucket size.
			 * Spins past transient busy marks set by concurrent operations.
			 * @return bucket size (0 when the slot is empty or holds an array, not a list)
			 */
			size_type peek(node_pointer node, size_type index, size_type position)
			{
				atomic_marked_node& slot(node->at(index));
				for (;;) {
					marked_node x_node;
					do {
						x_node = slot.load();
					} while (nullptr != x_node && (x_node.mark() & atomic_hash_table::node_is_busy) == atomic_hash_table::node_is_busy);
					if (nullptr == x_node || x_node.mark() == atomic_hash_table::node_is_array) {
						return 0;
					}
					if (!slot.compare_exchange_strong(x_node, marked_node(x_node.ptr(), atomic_hash_table::node_is_busy))) {
						continue;
					}
					list_node* const list(atomic_hash_table::adapter(x_node.ptr()).list);
					const size_type list_size(list->size());
					if (position < list_size) {
						typename std::list<typename list_node::data>::iterator data_it(list->at_position(position));
						_key = data_it->first;
						_value = data_it->second;
					}
					marked_node busy_state(x_node.ptr(), atomic_hash_table::node_is_busy);
					slot.compare_exchange_strong(busy_state, marked_node(x_node.ptr(), atomic_hash_table::node_is_list));
					return list_size;
				}
			}

			bool move(const iterator_direction direction, bool go_deep = true)
			{
				if (!_node) {
					return false;
				}

				/// @brief Phase 1: step within the current multi-entry bucket before navigating to another slot. Single-entry buckets fall through unchanged.
				if (go_deep) {
					if (iterator_direction::forward == direction) {
						if (_position + 1 < peek(_node, _index, _position + 1)) {
							++_position;
							return true;
						}
					} else if (_position > 0) {
						peek(_node, _index, _position - 1);
						--_position;
						return true;
					}
				}

				marked_node x_node(nullptr);
				node_pointer node(_node);
				std::size_t index(_index);

				/// @brief Переходим
				do {
					iterator_end end(iterator_end::middle);
					if (go_deep) {
						switch (direction) {
							case iterator_direction::forward:
								if (node->size() <= ++index) {
									index = node->size();
									end = iterator_end::last;
								}
								break;

							case iterator_direction::backward:
								if (0 >= index) {
									index = 0;
									end = iterator_end::first;
								} else {
									--index;
								}
								break;
						}
					} else {
						go_deep = true;
					}

					if (iterator_end::middle != end) {
						if (nullptr == node->get_parent_array()) { /// < Нет родительского массива, значит уперлись в конец/начало
							_node = node;
							_index = index;
							_position = 0;
							_value.reset();
							_key = key_type{};

							/// @note Начало, на самом деле, не настоящее начало - пойдем искать реально первый элемент
							if (iterator_end::first == end) {
								return move(iterator_direction::forward, false);
							}

							return false;
						}

						/// @brief Возвращаемся к родительскому массиву
						index = node->get_parent_index();
						node = node->get_parent_array();
						continue;
					}

					do {
						x_node = node->at(index).load();
					} while (nullptr != x_node && (x_node.mark() & atomic_hash_table::node_is_busy) == atomic_hash_table::node_is_busy);
					if (nullptr == x_node) { /// < Пустая ячейка, переходим к следующей
						continue;
					} else if (x_node.mark() == atomic_hash_table::node_is_array) { /// < Уходим внутрь массива
						node = adapter(x_node.ptr()).array;
						index = (iterator_direction::forward == direction ? 0 : node->size() - 1);
						go_deep = false;
						x_node.reset();
						continue;
					} else { /// < Данные
						atomic_marked_node& slot = node->at(index);
						if (!slot.compare_exchange_strong(x_node, marked_node(x_node.ptr(), atomic_hash_table::node_is_busy))) {
							go_deep = false;
							x_node.reset();
							continue;
						}
						adapter list_adapter(x_node.ptr());
						/// @brief Multi-entry buckets: forward lands on the first element, backward on the last; Phase 1 walks the rest.
						const size_type list_size(list_adapter.list->size());
						const bool found(list_size > 0);
						if (found) {
							const size_type position(iterator_direction::forward == direction ? 0 : (list_size - 1));
							typename std::list<typename list_node::data>::iterator list_iterator(list_adapter.list->at_position(position));
							_key = list_iterator->first;
							_value = list_iterator->second;
							_node = node;
							_index = index;
							_position = position;
						}
						marked_node busy_state(x_node.ptr(), atomic_hash_table::node_is_busy);
						slot.compare_exchange_strong(busy_state, marked_node(x_node.ptr(), atomic_hash_table::node_is_list));
						if (found) {
							return true;
						}
						x_node.reset();
						continue;
					}
				} while (nullptr == x_node);

				return false;
			}

		public:
			std::string path() const
			{
				const abstract_node* node(_node->at(_index).load().all());
				std::size_t index(_index);

				std::stringstream stream;
				while (nullptr != node) {
					index = node->get_parent_index();
					node = node->get_parent_array();
					stream << "/" << std::hex << reinterpret_cast<std::uintptr_t>(node) << std::dec << ":" << index;
				}

				return stream.str();
			}
		/** @} */

		/** @name Operators */
		/** @{ */
		public:
			basic_iterator<IsConst>& operator=(const basic_iterator<IsConst>& origin)
			{
				_node = origin._node;
				_index = origin._index;
				_position = origin._position;
				_value = origin._value;
				_key = origin._key;
				return *this;
			}

			bool operator==(const basic_iterator<IsConst>& origin) const
			{
				return (origin._node == _node && origin._index == _index && origin._position == _position);
			}

			bool operator!=(const basic_iterator<IsConst>& origin) const
			{
				return (origin._node != _node || origin._index != _index || origin._position != _position);
			}

			basic_iterator<IsConst>& operator++()
			{
				move(iterator_direction::forward);

				return *this;
			}

			basic_iterator<IsConst> operator++(int)
			{
				basic_iterator<IsConst> retval(*this);
				operator++();

				return retval;
			}

			basic_iterator<IsConst>& operator--()
			{
				move(iterator_direction::backward);

				return *this;
			}

			basic_iterator<IsConst> operator--(int)
			{
				basic_iterator<IsConst> retval(*this);
				operator--();

				return retval;
			}

			/**
			 * @note SFINAE enables the const dereference operator or the non const variant depending on bool Const parameter
			 */
			template<bool _C = IsConst>
			std::enable_if_t<_C, reference>
			operator*() const
			{
				return _pair;
			}

			template<bool _C = IsConst>
			std::enable_if_t<!_C, reference>
			operator*()
			{
				return _pair;
			}

			template<bool _C = IsConst>
			std::enable_if_t<_C, pointer>
			operator->() const
			{
				return &_pair;
			}

			template<bool _C = IsConst>
			std::enable_if_t<!_C, pointer>
			operator->()
			{
				return &_pair;
			}
		/** @} */
		};
		using iterator = basic_iterator<false>;
		using const_iterator = basic_iterator<true>;
		using reverse_iterator = typename std::reverse_iterator<iterator>;
		using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;

		struct node_traits
		{
			static std::size_t calculate_mask(std::size_t size)
			{
				std::size_t retval(1);

				return (retval << size) - 1;
			}

			node_traits(std::size_t key_size)
			:
				key_size(key_size),
				array_size(std::size_t(1) << key_size),
				mask(calculate_mask(key_size))
			{

			}

			const std::size_t key_size;
			const std::size_t array_size;
			const std::size_t mask;
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_hash_table(std::size_t head_key_size = atomic_hash_table::headkey_size, std::size_t slot_key_size = atomic_hash_table::slotkey_size)
		:
			_slot_traits((slot_key_size < 2) ? 2 : slot_key_size),
			_head_traits(atomic_hash_table::calculate_head_key_size(head_key_size, _slot_traits.key_size)),

			_head_node(_head_traits.array_size, nullptr, 0)
		{

		}

		~atomic_hash_table() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const node_traits _slot_traits;
		const node_traits _head_traits;

		mutable array_node _head_node;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
	/** @} */

	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		/**
		 * @brief insert (legacy keyless form, K=uintptr_t only)
		 * @param item Key derived via std::hash<mapped_type>.
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename U = key_type, std::enable_if_t<std::is_same_v<U, std::uintptr_t>, int> = 0>
		std::pair<iterator, bool> insert(mapped_type item, bool overwrite = true)
		{
			return insert(atomic_hash_table::extract_key(item), item, overwrite);
		}

		/**
		 * @brief insert
		 * @param item
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		std::pair<iterator, bool> insert(key_type key, mapped_type item, bool overwrite = true)
		{
			const std::size_t hash(hasher{}(key));

			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));

			/// @note Новый элемент
			std::unique_ptr<list_node> item_node(new list_node(key, path, item, &_head_node, i));

			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; ) {
				bool null_load_retry = false;
				marked_node dummy;
				/// @brief Пытаемся заменить новый элемент(item_node) на nullptr в ячейке array_node.
				/// @brief Insert with busy mark so erase() cannot delete the node before we copy its value into the return iterator.
				if (atom->compare_exchange_strong(dummy, marked_node(static_cast<abstract_node*>(item_node.get()), atomic_hash_table::node_is_busy))) {
					list_node* const raw_ptr = item_node.release();
					iterator retval(raw_ptr, raw_ptr->at_position(0));
					marked_node busy_state(static_cast<abstract_node*>(raw_ptr), atomic_hash_table::node_is_busy);
					if (!atom->compare_exchange_strong(busy_state, marked_node(static_cast<abstract_node*>(raw_ptr), atomic_hash_table::node_is_list))) {
						return std::make_pair(end(), false);
					}
					return std::make_pair(retval, true);
				} do {
					/// @note Не получилось поменять, значит там уже есть либо данные, либо массив
					marked_node target_node(atom->load());
					if (nullptr == target_node) {
						/**
						 * @brief The slot was erased between the failed CAS and this load.
						 * The slot is free — retry the CAS without consuming the fail budget.
						 * Not incrementing fail_counter ensures we keep trying until we win
						 * the free slot rather than giving up spuriously.
						 */
						null_load_retry = true;
						break;
					}

					if (target_node.mark() == atomic_hash_table::node_is_busy) { /// < Идет процесс преобразования в массив
						continue;
					} else if (target_node.mark() == atomic_hash_table::node_is_array) { /// < В этом слоте массив
						/// @note Погружаемся!
						i = path & _slot_traits.mask;
						path >>= _slot_traits.key_size;

						array_node* intermediate_array(atomic_hash_table::adapter(target_node.ptr()).array);
						atom = &(intermediate_array->at(i));

						/// @brief Обновим родительские связи
						item_node->set_parent(intermediate_array, i);
						item_node->shift(_slot_traits.key_size, _slot_traits.mask);

						/// @brief А есть куда погружаться? Нужно либо заменить текущий или(если ключи не совпадают) вставить в этот же слот
						if ((h + _slot_traits.key_size) >= hash_length) {
							target_node = atom->load();
							if (target_node.mark() == atomic_hash_table::node_is_list) {
								if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
									/// @note Гм, что-то изменилось пока мы готовились - нужно идти на еще один круг
									continue;
								}
								list_node* data_list(atomic_hash_table::adapter(target_node.ptr()).list);

								typename std::list<typename list_node::data>::iterator emplace_iterator(data_list->end());
								bool is_inserted(false);
								std::tie(emplace_iterator, is_inserted) = overwrite ? data_list->emplace(key, item) : data_list->try_emplace(key, item);

								/// @brief Build result before clearing busy: erase() cannot touch data_list until after the CAS below, so the iterator reads from live memory.
								std::pair<iterator, bool> retval(
									data_list->end() == emplace_iterator
										? std::make_pair(end(), false)
										: std::make_pair(iterator(data_list, emplace_iterator), is_inserted)
								);
								target_node |= atomic_hash_table::node_is_busy;
								if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_list))) {
									/// @xxx Если вернуть состояние ноды не удается - пиши пропало
//										fail_counter = _fail_counter;
									return std::make_pair(end(), false);
								}
								return retval;
							}
						}
						break;
					} else { /// < Это слот с данным
						if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
							/// @note Гм, что-то изменилось пока мы готовились - нужно идти на еще один круг
							continue;
						}
						/// @brief Будем менять только если он в режиме трансформации, т.е. busy
						target_node |= atomic_hash_table::node_is_busy;

						/// @note Текущая list_node с элементами
						list_node* movable_node(atomic_hash_table::adapter(target_node.ptr()).list);

						/**
						 * @brief Separate-chaining inside this bucket instead of structural split when:
						 * 1) ключ уже в этом листе — inline emplace/update;
						 * 2) оставшиеся биты хэша совпадают (movable и item неразличимы) —  split бесполезен, он никогда не разведёт их по разным слотам;
						 * 3) биты хэша исчерпаны (достигли максимальной глубины дерева).
						 * Это устраняет misrouting и потерю ключей при полной коллизии std::hash<K>
						 */
						if (
							movable_node->at_key(key) != movable_node->end()
							|| movable_node->hash_value() == item_node->hash_value()
							|| (h + _slot_traits.key_size) >= hash_length
						) {
							typename std::list<typename list_node::data>::iterator emplace_iterator(movable_node->end());
							bool is_inserted(false);
							std::tie(emplace_iterator, is_inserted) = overwrite ? movable_node->emplace(key, item) : movable_node->try_emplace(key, item);
							/// @brief Build result before clearing busy: erase() cannot touch movable_node until after the CAS below, so the iterator reads from live memory.
							std::pair<iterator, bool> retval
							(
								(movable_node->end() == emplace_iterator)
								? std::make_pair(end(), false)
								: std::make_pair(iterator(movable_node, emplace_iterator), is_inserted)
							);
							if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_list))) {
								return std::make_pair(end(), false);
							}
							return retval;
						}

						array_node* origin_node(movable_node->get_parent_array());
						std::size_t origin_index(movable_node->get_parent_index());

						/// @brief Создадим новый array_node ссылаясь на arary_node-контейнер(берем у перемещаемой list_node)
						std::unique_ptr<array_node> adopt_node(new array_node(_slot_traits.array_size, origin_node, origin_index));

						/// @brief Усыновляем movable_node новым candidate_node
						std::size_t movable_index(movable_node->shift(_slot_traits.key_size, _slot_traits.mask));
						movable_node->set_parent(adopt_node.get(), movable_index);
						adopt_node->at(movable_index).store(marked_node(movable_node));

						/// @brief Заменим текущий list_node
						if (atom->compare_exchange_strong(target_node, marked_node(adopt_node.get(), atomic_hash_table::node_is_array))) {
							adopt_node.release();
							break;
						} else {
							/// @brief Вернем как было
							movable_node->shift_back(_slot_traits.key_size, movable_index);
							movable_node->set_parent(origin_node, origin_index);

							if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr()))) {
								throw std::logic_error("Structural transform rollback failed: busy slot was modified by a concurrent operation");
							}
						}
					}
				} while (true);
				if (!null_load_retry) {
					h += _slot_traits.key_size;
				}
			}
			return std::make_pair(end(), false);
		}

		/**
		 * @brief emplace
		 * @param key_type
		 * @param args
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename ...TArgs>
		std::pair<iterator, bool> emplace(key_type key, TArgs&& ...args)
		{
			return insert(key, mapped_type(std::forward<TArgs>(args)... ));
		}

		/**
		 * @brief emplace (legacy keyless form, K=uintptr_t only)
		 * @param args
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename ...TArgs, typename U = key_type, std::enable_if_t<std::is_same_v<U, std::uintptr_t>, int> = 0>
		std::pair<iterator, bool> emplace(TArgs&& ...args)
		{
			return insert(mapped_type(std::forward<TArgs>(args)... ));
		}

		/**
		 * @brief try_emplace If a key equivalent to k already exists in the container, does nothing.
		 * @param key_type
		 * @param args
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename ...TArgs>
		std::pair<iterator, bool> try_emplace(key_type key, TArgs&& ...args)
		{
			return insert(key, mapped_type(std::forward<TArgs>(args)... ), false);
		}

		/**
		 * @brief try_emplace If a key equivalent to k already exists in the container, does nothing.
		 * Legacy keyless form, K=uintptr_t only.
		 * @param args
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename ...TArgs, typename U = key_type, std::enable_if_t<std::is_same_v<U, std::uintptr_t>, int> = 0>
		std::pair<iterator, bool> try_emplace(TArgs&& ...args)
		{
			return insert(mapped_type(std::forward<TArgs>(args)... ), false);
		}

		/**
		 * @brief Removes the entry for key and returns an iterator holding a snapshot
		 * of the extracted key/value. Returns end() if the key is not present — never
		 * throws. Mirrors the iterator-returning style of erase().
		 *
		 * The returned iterator's _key and _value are stable copies of the extracted
		 * entry, captured before the underlying list_node is cleared. it->second
		 * yields the extracted mapped_type; it == end() signals absence.
		 */
		iterator extract(const key_type& key)
		{
			std::size_t hash(hasher{}(key));
			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));
			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; ) {
				bool retry(false);
				marked_node target_node;
				do {
					target_node = atom->load();
				} while (target_node.mark() == atomic_hash_table::node_is_busy);

				if (nullptr == target_node) {
					return end();
				}

				if (target_node.mark() == atomic_hash_table::node_is_array) {
					i = path & _slot_traits.mask;
					path >>= _slot_traits.key_size;
					atom = &(atomic_hash_table::adapter(target_node.ptr()).array->at(i));
				} else {
					/** @brief Acquire the bucket under busy, extract only the matching key
					 * (separate chaining): null the slot only when the bucket becomes empty,
					 * otherwise restore it as a list so co-located colliding keys survive.
					 */
					if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
						retry = true;
					} else {
						list_node* node(atomic_hash_table::adapter(target_node.ptr()).list);
						typename std::list<typename list_node::data>::iterator key_it(node->at_key(key));
						if (node->end() == key_it) {
							marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
							atom->compare_exchange_strong(busy_state, marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list));
							return end();
						}
						iterator retval(node, key_it);
						node->erase(key);
						marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
						atom->compare_exchange_strong(
							busy_state,
							node->size() == 0
								? marked_node(nullptr)
								: marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list)
						);
						return retval;
					}
				}

				if (!retry) {
					h += _slot_traits.key_size;
				}
			}
			return end();
		}

		/**
		 * @brief Whole-bucket granularity: for a multi-entry (hash-collision) bucket this
		 * removes every co-located key at once, not just the one at i. For per-key
		 * removal use erase(key) / extract(key).
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const_iterator i)
		{
			if (nullptr == i._node || i._node->size() <= i._index) {
				return iterator(i._node, iterator_end::last);
			}
			atomic_marked_node& slot = i._node->at(i._index);
			marked_node current;
			do {
				current = slot.load();
			} while (nullptr != current && (current.mark() & atomic_hash_table::node_is_busy) == atomic_hash_table::node_is_busy);
			if (nullptr == current || current.mark() != atomic_hash_table::node_is_list) {
				return iterator(i._node, i._index);
			}
			if (slot.compare_exchange_strong(current, marked_node(nullptr))) {
				atomic_hash_table::adapter(current.ptr()).list->clear();
			}
			return iterator(i._node, i._index);
		}

		/**
		 * @brief Caller must ensure no concurrent structural modifications occur during
		 * range iteration; otherwise last may never compare equal and the loop
		 * will not terminate.
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const_iterator first, const_iterator last)
		{
			for (const_iterator i = first; i != last; i++) {
				erase(i);
			}
			return iterator(last._node, last._index);
		}

		/**
		 * @brief Whole-bucket granularity: for a multi-entry (hash-collision) bucket this
		 * removes every co-located key at once, not just the one at i. For per-key
		 * removal use erase(key) / extract(key).
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(iterator i)
		{
			if (nullptr == i._node || i._node->size() <= i._index) {
				return i;
			}
			atomic_marked_node& slot = i._node->at(i._index);
			marked_node current;
			do {
				current = slot.load();
			} while (nullptr != current && (current.mark() & atomic_hash_table::node_is_busy) == atomic_hash_table::node_is_busy);
			if (nullptr == current || current.mark() != atomic_hash_table::node_is_list) {
				return std::next(i);
			}
			if (slot.compare_exchange_strong(current, marked_node(nullptr))) {
				atomic_hash_table::adapter(current.ptr()).list->clear();
			}
			return std::next(i);
		}

		/**
		 * @brief Same caveat as erase(const_iterator, const_iterator): no concurrent structural
		 * modifications during range iteration.
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const iterator& first, const iterator& last)
		{
			for (iterator i = first; i != last; i++) {
				erase(i);
			}
			return last;
		}

		/**
		 * @brief Atomically replaces the mapped_type stored under key with desired iff
		 * the current stored value equals expected. CAS semantics, mirrors
		 * std::atomic<T>::compare_exchange_strong(expected, desired).
		 *
		 * Equality is mapped_type's operator==. For shared_ptr<T> this compares the
		 * managed pointer — exactly the semantics needed for RCU-style CAS loops.
		 *
		 * The operation runs under the slot's busy mark, so it is serialized with
		 * concurrent find()/insert()/exchange()/erase() on the same slot.
		 *
		 * @param key Lookup key.
		 * @param expected In/out: value the caller expects to find. On success unchanged;
		 * on failure updated to the actual current value.
		 * @param desired Value to store if expected matches.
		 *
		 * @return true if the swap succeeded; false if the key is absent or the stored
		 * value did not equal expected (expected then holds the actual value).
		 */
		bool exchange(const key_type& key, mapped_type& expected, const mapped_type& desired)
		{
			std::size_t hash(hasher{}(key));
			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));

			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; ) {
				bool retry_same_slot(false);
				marked_node target_node;
				do {
					target_node = atom->load();
				} while (target_node.mark() == atomic_hash_table::node_is_busy);

				if (nullptr == target_node) {
					return false;
				}

				if (target_node.mark() == atomic_hash_table::node_is_array) {
					i = path & _slot_traits.mask;
					path >>= _slot_traits.key_size;
					atom = &(atomic_hash_table::adapter(target_node.ptr()).array->at(i));
				} else {
					if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
						retry_same_slot = true;
					} else {
						list_node* const node(atomic_hash_table::adapter(target_node.ptr()).list);
						typename std::list<typename list_node::data>::iterator key_it(node->at_key(key));
						bool retval(false);
						if (node->end() != key_it) {
							if (key_it->second == expected) {
								key_it->second = desired;
								retval = true;
							} else {
								expected = key_it->second;
							}
						}
						marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
						atom->compare_exchange_strong(busy_state, marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list));
						return retval;
					}
				}

				if (!retry_same_slot) {
					h += _slot_traits.key_size;
				}
			}
			return false;
		}

		/**
		 * @brief erase
		 * @param key
		 * @return
		 */
		size_type erase(const key_type& key)
		{
			std::size_t hash(hasher{}(key));
			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));
			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; ) {
				bool retry(false);
				marked_node target_node;
				do {
					target_node = atom->load();
				} while (target_node.mark() == atomic_hash_table::node_is_busy);

				if (nullptr == target_node) {
					return 0;
				}

				if (target_node.mark() == atomic_hash_table::node_is_array) {
					i = path & _slot_traits.mask;
					path >>= _slot_traits.key_size;
					atom = &(atomic_hash_table::adapter(target_node.ptr()).array->at(i));
				} else {
					/**
					 * @brief Acquire the bucket under busy and remove only the matching key
					 * (separate chaining): null the slot when the bucket becomes empty,
					 * otherwise restore it as a list so co-located colliding keys survive.
					 * Retry if CAS-to-busy fails due to a transient mark from a concurrent op.
					 */
					if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
						retry = true;
					} else {
						list_node* node(atomic_hash_table::adapter(target_node.ptr()).list);
						const bool removed(node->erase(key));
						marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
						atom->compare_exchange_strong(
							busy_state,
							node->size() == 0
								? marked_node(nullptr)
								: marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list)
						);
						return removed ? 1 : 0;
					}
				}

				if (!retry) {
					h += _slot_traits.key_size;
				}
			}
			return 0;
		}

		/**
		 * @brief clear
		 */
		void clear()
		{
			erase(begin(), end());
		}

		/**
		 * @brief dispose
		 * @param key
		 * @return
		 */
		bool dispose(const key_type& key)
		{
			return erase(key) > 0;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	private:
		iterator find_node(const key_type& key) const
		{
			std::size_t hash(hasher{}(key));
			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));

			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; ) {
				bool retry_same_slot(false);
				marked_node target_node;
				do {
					target_node = atom->load();
				} while (target_node.mark() == atomic_hash_table::node_is_busy);

				if (nullptr == target_node) {
					return iterator(&_head_node, iterator_end::last);
				}

				if (target_node.mark() == atomic_hash_table::node_is_array) {
					i = path & _slot_traits.mask;
					path >>= _slot_traits.key_size;
					atom = &(atomic_hash_table::adapter(target_node.ptr()).array->at(i));
				} else {
					if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
						retry_same_slot = true;
					} else {
						list_node* const node(atomic_hash_table::adapter(target_node.ptr()).list);
						typename std::list<typename list_node::data>::iterator key_it(node->at_key(key));
						iterator result((node->end() == key_it) ? iterator(&_head_node, iterator_end::last) : iterator(node, key_it));
						marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
						atom->compare_exchange_strong(busy_state, marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list));
						return result;
					}
				}

				if (!retry_same_slot) {
					h += _slot_traits.key_size;
				}
			}
			return iterator(&_head_node, iterator_end::last);
		}

	public:
		iterator begin()
		{
			return iterator(&_head_node, iterator_end::first);
		}

		const_iterator begin() const
		{
			return const_iterator(&_head_node, iterator_end::first);
		}

		const_iterator cbegin() const
		{
			return const_iterator(&_head_node, iterator_end::first);
		}

		iterator end()
		{
			return iterator(&_head_node, iterator_end::last);
		}

		const_iterator end() const
		{
			return const_iterator(&_head_node, iterator_end::last);
		}

		const_iterator cend() const
		{
			return const_iterator(&_head_node, iterator_end::last);
		}

		iterator find(const key_type& key)
		{
			return find_node(key);
		}

		const_iterator find(const key_type& key) const
		{
			return const_iterator(find_node(key));
		}

		mapped_type get(const key_type& key) const
		{
			std::size_t hash(hasher{}(key));
			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));

			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; ) {
				bool retry_same_slot(false);
				marked_node target_node;
				do {
					target_node = atom->load();
				} while (target_node.mark() == atomic_hash_table::node_is_busy);

				if (nullptr == target_node) {
					throw std::out_of_range("Key not found");
				} else if (target_node.mark() == atomic_hash_table::node_is_array) {
					i = path & _slot_traits.mask;
					path >>= _slot_traits.key_size;
					atom = &(atomic_hash_table::adapter(target_node.ptr()).array->at(i));
				} else {
					if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
						retry_same_slot = true;
					} else {
						list_node* const node(atomic_hash_table::adapter(target_node.ptr()).list);
						typename std::list<typename list_node::data>::const_iterator key_it(node->at_key(key));

						if (node->cend() == key_it) {
							marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
							atom->compare_exchange_strong(busy_state, marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list));
							throw std::out_of_range("Key not found");
						}

						mapped_type retval(key_it->second);
						marked_node busy_state(static_cast<abstract_node*>(node), atomic_hash_table::node_is_busy);
						atom->compare_exchange_strong(busy_state, marked_node(static_cast<abstract_node*>(node), atomic_hash_table::node_is_list));
						return retval;
					}
				}

				if (!retry_same_slot) {
					h += _slot_traits.key_size;
				}
			}
			throw std::out_of_range("Key not found");
		}

		size_type bucket_count() const noexcept
		{
			/// @note Not tracked; would require an atomic counter updated on every structural split.
			return 0;
		}

		size_type count(const key_type& key) const
		{
			return (find(key) != cend()) ? 1 : 0;
		}

		size_type size() const noexcept
		{
			return std::distance(cbegin(), cend());
		}

		bool empty() const noexcept
		{
			return cbegin() == cend();
		}
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using value_type = typename iterator::value_type;
		using reference = typename iterator::reference;
	/** @} */
	};
}

#endif
