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
#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <stdexcept>

namespace chaos {
	/**
	 * @brief Concurrent Multi-level Arrays: Wait-free Extensible Hash Maps
	 * @url http://samos-conference.com/Resources_Samos_Websites/Proceedings_Repository_SAMOS/2013/Files/2013-IC-20.pdf
	 */
	template <typename T>
	class atomic_hash_table
	{
	/** @name Aliases */
	/** @{ */
	public:
		using key_type = std::uintptr_t;
		using mapped_type = T;
		using size_type = std::size_t;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const std::size_t hash_size = sizeof(std::size_t) * 8;
		static const std::size_t headkey_size = 8; /// @note ^2
		static const std::size_t slotkey_size = 4; /// @note ^2
		static const std::uint16_t fail_limit = 100;

		static const std::uint8_t node_is_list = 0b00;
		static const std::uint8_t node_is_busy = 0b01;
		static const std::uint8_t node_is_array = 0b10;

		static key_type extract_key(const mapped_type& item)
		{
			return std::hash<mapped_type>()(item);
			//return reinterpret_cast<key_type>(item.get());
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
		using A = std::allocator<T>;

		class array_node;
		class list_node;

		class abstract_node
		{
		friend atomic_hash_table;

		/** @name Aliases */
		/** @{ */
		public:
		/** @} */

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
			std::size_t shift(std::size_t step, std::size_t mask)
			{
				std::size_t retval(_hash & mask);
				_hash >>= step;

				return retval;
			}

			std::size_t shift_back(std::size_t step, std::size_t index)
			{
				return (_hash = ((_hash << step) & index));
			}

			std::pair<typename std::list<data>::iterator, bool> emplace(key_type key, mapped_type value)
			{
				typename std::list<data>::iterator i(at_key(key));
				if (_list.end() != i) {
					i->second = value;
					return std::make_pair(i, false);
				}
				
				_list.emplace_back(key, value);
				return std::make_pair(_list.rbegin().base(), true);
			}
			
			std::pair<typename std::list<data>::iterator, bool> try_emplace(key_type key, mapped_type value)
			{
				typename std::list<data>::iterator i(at_key(key));
				if (_list.end() != i) {
					return std::make_pair(_list.end(), false);
				}
				
				_list.emplace_back(key, value);
				return std::make_pair(_list.rbegin().base(), true);
			}
		/** @} */
		};

		class array_node : public abstract_node
		{
		friend atomic_hash_table;

		/** @name Aliases */
		/** @{ */
		public:
		/** @} */

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

		/** @name Mutators */
		/** @{ */
		public:
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

		/** @name Procedures */
		/** @{ */
		private:
		/** @} */

		/** @name States */
		/** @{ */
		public:
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
			using node_pointer = typename std::conditional_t<IsConst, array_node const*, array_node*>;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			basic_iterator(node_pointer node, size_type index = 0)
			:
				_key(0),
				_value(nullptr),
				_pair(_key, _value),
				_node(node),
				_index(index)
			{
				move(iterator_direction::forward, false);
			}

			basic_iterator(node_pointer node, iterator_end end)
			:
				_key(0),
				_value(nullptr),
				_pair(_key, _value),
				_node(node),
				_index(iterator_end::first == end ? 0 : _node->size())
			{
				if (iterator_end::first == end) {
					move(iterator_direction::forward, false);
				}
			}

			basic_iterator(const basic_iterator<IsConst>& origin)
			:
				_key(origin._key),
				_value(origin._value),
				_pair(_key, _value),
				_node(origin._node),
				_index(origin._index)
			{

			}

			template <bool Const, std::enable_if_t<IsConst && Const != IsConst, int> = 0>
			basic_iterator(const basic_iterator<Const>& origin)
			:
				_key(origin._key),
				_value(origin._value),
				_pair(_key, _value),
				_node(origin._node),
				_index(origin._index)
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

			node_pointer _node; /// @todo Переехать на union array_node*/list_node*
			size_type _index;
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			bool move(const iterator_direction direction, bool go_deep = true)
			{
				if (!_node) {
					return false;
				}

				marked_node x_node(nullptr);
				node_pointer node(_node);
				std::size_t index(_index);

				/// Переходим
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
							_value.reset();
							_key = 0;

							/// Начало, на самом деле, не настоящее начало - пойдем искать реально первый элемент
							if (iterator_end::first == end) {
								return move(iterator_direction::forward, false);
							}

							return false;
						}

						/// Возвращаемся к родительскому массиву
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
						adapter list_adapter(x_node.ptr());
						/// @todo Может быть и больше в одной и той же ячейке
						typename std::list<typename list_node::data>::const_iterator list_iterator(list_adapter.list->at_position(0));
						if (list_iterator == list_adapter.list->cend()) {
							/// @xxx WTF?
							continue;
						}
						_key = list_iterator->first;
						_value = list_iterator->second;
						_node = node;
						_index = index;

						return true;
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
				_value = origin._value;
				_key = origin._key;

				return *this;
			}

			bool operator==(const basic_iterator<IsConst>& origin) const
			{
				return (origin._node == _node && origin._index == _index);
			}

			bool operator!=(const basic_iterator<IsConst>& origin) const
			{
				return (origin._node != _node || origin._index != _index);
			}

			bool operator<(const basic_iterator<IsConst>&) const
			{
				return false; /// ???
			}

			bool operator>(const basic_iterator<IsConst>&) const
			{
				return false; /// ???
			}

			bool operator<=(const basic_iterator<IsConst>&) const
			{
				return false; /// ???
			}

			bool operator>=(const basic_iterator<IsConst>&) const
			{
				return false; /// ???
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

/*
			iterator& operator+=(size_type); //optional
			iterator operator+(size_type) const; //optional
			friend iterator operator+(size_type, const iterator&); //optional
			iterator& operator-=(size_type); //optional
			iterator operator-(size_type) const; //optional
			difference_type operator-(iterator) const; //optional
*/

			/**
			 * SFINAE enables the const dereference operator or the non const variant depending on bool Const parameter
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

	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_hash_table(std::size_t head_key_size = atomic_hash_table::headkey_size, std::size_t slot_key_size = atomic_hash_table::slotkey_size, std::uint16_t fail_counter = atomic_hash_table::fail_limit)
		:
			_slot_traits((slot_key_size < 2) ? 2 : slot_key_size),
			_head_traits(atomic_hash_table::calculate_head_key_size(head_key_size, _slot_traits.key_size)),

			_head_node(_head_traits.array_size, nullptr, 0),

			_fail_counter(fail_counter)
		{

		}

		~atomic_hash_table() = default;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		T& operator[](const std::uintptr_t& key)
		{
			return at(key);
		}

		T& operator[](std::uintptr_t&& key)
		{
			return at(key);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const node_traits _slot_traits;
		const node_traits _head_traits;

		array_node _head_node;

		const std::uint16_t _fail_counter;
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
		 * @brief insert
		 * @param item
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
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
			std::uint16_t fail_counter(0);
			const std::size_t hash(key); /// @xxx или с <T>item'a?

			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			atomic_marked_node* atom(&(_head_node.at(i)));

			/// Новый элемент
			std::unique_ptr<list_node> item_node(new list_node(key, path, item, &_head_node, i));

			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h < hash_length; h += _slot_traits.key_size) {
				if (fail_counter >= _fail_counter) {
					break;
				}

				marked_node dummy;
				/// Пытаемся заменить новый элемент(item_node) на nullptr в ячейке array_node
				if (atom->compare_exchange_strong(dummy, marked_node(static_cast<abstract_node*>(item_node.get())))) {
					iterator retval(iterator(item_node->get_parent_array(), i));
					item_node.release();
					return std::make_pair(retval, true);
				} else {
					do {
						/// Не получилось поменять, значит там уже есть либо данные, либо массив
						marked_node target_node(atom->load());
						if (nullptr == target_node) {
							fail_counter++;
							return std::make_pair(end(), false);
						}

						if (target_node.mark() == atomic_hash_table::node_is_busy) { /// < Идет процесс преобразования в массив
							fail_counter++; /// @xxx В общем-то, не обязательно, но ведь есть очень тонкое место
							continue;
						} else if (target_node.mark() == atomic_hash_table::node_is_array) { /// < В этом слоте массив
							/// Погружаемся!
							i = path & _slot_traits.mask;
							path >>= _slot_traits.key_size;

							array_node* intermediate_array(atomic_hash_table::adapter(target_node.ptr()).array);
							atom = &(intermediate_array->at(i));

							/// Обновим родительские связи
							item_node->set_parent(intermediate_array, i);
							item_node->shift(_slot_traits.key_size, _slot_traits.mask);
							
							/// А есть куда погружаться? Нужно либо заменить текущий или(если ключи не совпадают) вставить в этот же слот
							if ((h + _slot_traits.key_size) >= hash_length) {
								target_node = atom->load();
								if (target_node.mark() == atomic_hash_table::node_is_list) {
									if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
										/// Гм, что-то изменилось пока мы готовились - нужно идти на еще один круг
										continue;
									}
									list_node* data_list(atomic_hash_table::adapter(target_node.ptr()).list);

									typename std::list<typename list_node::data>::iterator emplace_iterator(data_list->end());
									bool is_inserted(false);
									std::tie(emplace_iterator, is_inserted) = overwrite ? data_list->emplace(key, item) : data_list->try_emplace(key, item);
									
									target_node |= atomic_hash_table::node_is_busy;
									if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_list))) {
										/// @xxx Если вернуть состояние ноды не удается - пиши пропало
//										fail_counter = _fail_counter;
										return std::make_pair(end(), false);
									}
									return data_list->end() == emplace_iterator ? std::make_pair(end(), false) : std::make_pair(iterator(data_list->get_parent_array(), data_list->get_parent_index()), is_inserted);
								}
							}
							break;
						} else { /// <  Это слот с данным
							if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr(), atomic_hash_table::node_is_busy))) {
								/// Гм, что-то изменилось пока мы готовились - нужно идти на еще один круг
								continue;
							}
							/// Будем менять только если он в режиме трансформации, т.е. busy
							target_node |= atomic_hash_table::node_is_busy;

							/// Текущая list_node с элементами
							list_node* movable_node(atomic_hash_table::adapter(target_node.ptr()).list);
							array_node* origin_node(movable_node->get_parent_array());
							std::size_t origin_index(movable_node->get_parent_index());

							/// Создадим новый array_node ссылаясь на arary_node-контейнер(берем у перемещаемой list_node)
							std::unique_ptr<array_node> adopt_node(new array_node(_slot_traits.array_size, origin_node, origin_index));

							/// Усыновляем movable_node новым candidate_node
							std::size_t movable_index(movable_node->shift(_slot_traits.key_size, _slot_traits.mask));
							movable_node->set_parent(adopt_node.get(), movable_index);
							adopt_node->at(movable_index).store(marked_node(movable_node));

							/// Заменим текущий list_node
							if (atom->compare_exchange_strong(target_node, marked_node(adopt_node.get(), atomic_hash_table::node_is_array))) {
								adopt_node.release();
								break;
							} else {
								/// @ocd Хочется верить, что этого не случится
								fail_counter++;
								assert(false);

								/// Вернем как было
								movable_node->shift_back(_slot_traits.key_size, movable_index);
								movable_node->set_parent(origin_node, origin_index);

								if (!atom->compare_exchange_strong(target_node, marked_node(target_node.ptr()))) {
									/// @xxx Game, как говорится, over
									assert(false);
									return std::make_pair(end(), false);
								}
							}
						}
					} while (true);
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
			return insert(key, T(std::forward<TArgs>(args)... ));
		}

		/**
		 * @brief emplace
		 * @param args
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename ...TArgs>
		std::pair<iterator, bool> emplace(TArgs&& ...args)
		{
			return insert(T(std::forward<TArgs>(args)... ));
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
			return insert(key, T(std::forward<TArgs>(args)... ), false);
		}

		/**
		 * @brief try_emplace If a key equivalent to k already exists in the container, does nothing.
		 * @param args
		 * @return pair<iterator, bool> True for Insertion, False for No Insertion.
		 */
		template<typename ...TArgs>
		std::pair<iterator, bool> try_emplace(TArgs&& ...args)
		{
			return insert(T(std::forward<TArgs>(args)... ), false);
		}

		mapped_type extract(const key_type& key)
		{
			list_node* node(get_node(key));

			if (nullptr == node) {
				return nullptr;
			}

			/// @todo Или, всего лишь, удалить один элемент из списка
			mapped_type retval(node->at_position(0)->second);
			marked_node obsolete(node->get_parent_array()->at(node->get_parent_index()).exchange(nullptr));
			if (nullptr != obsolete) {
				obsolete.dispose();
			}

			return retval;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const_iterator i)
		{
			std::stack<std::size_t> path;

			array_node* array(i._node);
			while (nullptr != array) {
				path.push(array->get_parent_index());
				array = array->get_parent_array();
			}

			array = &_head_node;
			while (!path.empty()) {
				marked_node node(array->at(path.top()).load());
				if (nullptr != node && node.mark() == atomic_hash_table::node_is_array) {
					array = adapter(node.ptr()).array;
				}
				path.pop();
			}

			/// @note Защита от непредвиденного: спустились вниз до того же массива, на который показывает итератор?
			if (array == i._node) {
				/// @todo Или, всего лишь, удалить один элемент из списка
				marked_ptr obsolete(array->at(i._index).exchange(nullptr));
				if (nullptr != obsolete) {
					obsolete.dispose();
					return ++i;
				}
			}

			return i;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const_iterator first, const_iterator last)
		{
			for (const_iterator i = first; i != last; i++) {
				erase(i);
			}

			return ++last;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(iterator i)
		{
			if (nullptr == i._node || i._node->size() <= i._index) {
				return i;
			}

			/// @todo Или, всего лишь, удалить один элемент из списка
			marked_ptr obsolete(i._node->at(i._index).exchange(nullptr));
			if (nullptr != obsolete) {
				delete obsolete.ptr();
			}

			return ++i;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(iterator first, iterator last)
		{
			for (iterator i = first; i != last; i++) {
				erase(i);
			}

			return ++last;
		}

		/**
		 * @brief erase
		 * @param key
		 * @return
		 */
		size_type erase(const key_type& key)
		{
			list_node* node(get_node(key));

			if (nullptr == node) {
				return 0;
			}

			/// @todo Или, всего лишь, удалить один элемент из списка
			marked_ptr obsolete(node->get_parent_array()->at(node->get_parent_index()).exchange(nullptr));
			if (nullptr != obsolete) {
				obsolete.dispose();
				return 0;
			}

			return 1;
		}

		/**
		 * @brief clear
		 */
		void clear()
		{
			iterator b(begin());
			iterator e(end());
			erase(b, e);
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
		list_node* get_node(const key_type& key) const
		{
			std::size_t hash(std::hash<std::uintptr_t>()(key));
			std::size_t i = hash & _head_traits.mask;
			std::size_t path(hash >> _head_traits.key_size);

			const atomic_marked_node* atom(&(_head_node.at(i)));

			for (std::size_t h = 0, hash_length(atomic_hash_table::hash_size - _head_traits.key_size); h <= hash_length; h += _slot_traits.key_size) {
				marked_node target_node(atom->load());

				if (target_node.mark() == atomic_hash_table::node_is_array) { /// < В этом слоте массив
					/// Погружаемся!
					i = path & _slot_traits.mask;
					path >>= _slot_traits.key_size;
					array_node* intermediate_array(atomic_hash_table::adapter(target_node.ptr()).array);
					atom = &(intermediate_array->at(i));
				} else { /// < Значит это слот с данным
					list_node* final_list(atomic_hash_table::adapter(target_node.ptr()).list);
					/// @todo Может быть больше одного элемента в листе с одинаковым хэшем, но с разными ключами

					return final_list;
				}
			}

			return nullptr;
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
			list_node* retval(get_node(key));

			return (nullptr == retval) ? end() : retval->end() == retval->at_key(key) ? end() : iterator(retval->get_parent_array(), retval->get_parent_index());
		}

		const_iterator find(const key_type& key) const
		{
			list_node* retval(get_node(key));

			return (nullptr == retval) ? cend() : retval->end() == retval->at_key(key) ? end() : const_iterator(retval->get_parent_array(), retval->get_parent_index());
		}

		T& at(const key_type& key)
		{
			list_node* retval(get_node(key));

			if (nullptr == retval) {
				throw std::out_of_range(std::to_string(key) + " is out of bounds");
			}

			typename std::list<typename list_node::data>::iterator i(retval->at_key(key));

			if (retval->end() == i) {
				throw std::out_of_range(std::to_string(key) + " is out of bounds");
			}

			return i->second;
		}

		const T& at(const key_type& key) const
		{
			list_node* retval(get_node(key));

			if (nullptr == retval) {
				throw std::out_of_range(std::to_string(key) + " is out of bounds");
			}

			typename std::list<typename list_node::data>::const_iterator i(retval->at_key(key));

			if (retval->cend() == i) {
				throw std::out_of_range(std::to_string(key) + " is out of bounds");
			}

			return i->second;
		}

		size_type bucket_count()
		{
			return 0;
		}

		size_type count(const key_type& key) const
		{
			list_node* retval(get_node(key));

			return (nullptr == retval) ? 0 : 1;
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

	/** @name Aliasses */
	/** @{ */
	public:		
		using value_type = typename iterator::value_type;
		using reference = typename iterator::reference;
	/** @} */
	};
}

#endif
