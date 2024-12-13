/**
 @file AtomicForwardList.hpp
 @date 27.07.2018
 @copyright Daniil A Megrabyan
 @author Daniil A Megrabyan
*/

#ifndef Cadabra_Chaos_AtomicForwardList_hpp
#define Cadabra_Chaos_AtomicForwardList_hpp

#include "MarkedPointer.hpp"

#include <atomic>
#include <cstdint>
#include <tuple>
#include <optional>
#include <stdexcept>

namespace chaos {
	/**
	 * @brief The atomic_forward_list class
	 * T has to be a kind of
	 */
	template <typename T>
	class atomic_forward_list
	{
	/** @name Aliases */
	/** @{ */
	public:
		using mapped_type = T;
		using size_type = std::size_t;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const std::uint8_t node_is_ready = 0b00;
		static const std::uint8_t node_is_busy = 0b01;
	/** @} */

	/** @name Classes */
	/** @{ */
	private:
		class data
		{
		/** @name Constructors */
		/** @{ */
		public:
			data() = delete;

			data(T&& origin)
			:
				value(std::move(origin))
			{

			}

			data(const T& origin)
			:
				value(origin)
			{

			}

			data(data&& origin)
			:
				value(std::move(origin.value)) /// @??? atomic exchange
			{

			}

			data(const data& origin) = delete;

			~data() = default;
		/** @} */

		/** @name Properties */
		/** @{ */
		public:
			T value;
		/** @} */
		};

		class node
		{
		friend atomic_forward_list;

		/** @name Constructors */
		/** @{ */
		private:
			node(T&& value)
			:
				_lead_node(nullptr),
				_value_data(new data(std::move(value)))
			{

			}

			node(const T& value)
			:
				_lead_node(nullptr),
				_value_data(new data(value))
			{

			}

			node()
			:
				_lead_node(nullptr)
			{

			}

			node(node&& origin)
			:
				_lead_node(origin._lead_node),
				_value_data(new data(origin._value_data))
			{

			}

		public:
			~node() = default;
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			std::atomic<marked_ptr<node>> _lead_node;
			std::shared_ptr<data> _value_data;
		/** @} */
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using marked_node = marked_ptr<node>;
		using atomic_marked_node = std::atomic<marked_node>;
		using iterator_tuple = std::tuple<node*, std::shared_ptr<data>, node*>;
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
		template<bool IsConst = false>
		class basic_iterator
		{
		friend atomic_forward_list;
		/** @name Aliases */
		/** @{ */
		public:
			using value_type = mapped_type;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::forward_iterator_tag;

			using reference = typename std::conditional_t<IsConst, mapped_type const&, mapped_type&>;
			using pointer = typename std::conditional_t<IsConst, mapped_type const*, mapped_type*>;
		/** @} */

		/** @name Constructors */
		/** @{ */
		private:
			basic_iterator(iterator_tuple&& value)
			:
				_rear_node(std::move(std::get<0>(value))),
				_target_node(std::move(std::get<2>(value))),
				_target_data(std::move(std::get<1>(value)))
			{

			}

		public:
			basic_iterator()
			:
				_rear_node(nullptr),
				_target_node(nullptr)
			{

			}

			basic_iterator(node* rear_node)
			:
				_rear_node(nullptr),
				_target_node(nullptr)
			{
				traverse(rear_node);
			}

			basic_iterator(const basic_iterator<IsConst>& origin)
			:
				_rear_node(origin._rear_node),
				_target_node(origin._target_node),
				_target_data(origin._target_data)
			{

			}

			basic_iterator(const basic_iterator<!IsConst>& origin)
			:
				_rear_node(origin._rear_node),
				_target_node(origin._target_node),
				_target_data(origin._target_data)
			{

			}

			~basic_iterator()
			{
			}
		/** @} */

		/** @name Properties */
		/** @{ */
		private:
			node* _rear_node;
			node* _target_node;
			std::shared_ptr<data> _target_data;
		/** @} */

		/** @name Procedures */
		/** @{ */
		private:
			bool traverse(node* base_node = nullptr)
			{
				if (!base_node) {
					if (!_target_node) {
						return false;
					}
					base_node = _target_node;
				}

				node* destination_node(nullptr);
				do {
					marked_ptr<node> spin_node(base_node->_lead_node.load());
					if (spin_node == nullptr) {
						_rear_node = nullptr;
						_target_node = nullptr;
						_target_data.reset();
						return true;
					}
					if (spin_node.mark() == node_is_ready) {
						std::shared_ptr<data> destination_data(spin_node->_value_data);
						if (destination_data) {
							_target_data.swap(destination_data);
							_target_node = spin_node.ptr();
							_rear_node = base_node;
						}
						return true;
					}
					base_node = spin_node.ptr();
				} while (!destination_node);
				return false;
			}
		/** @} */

		/** @name Operators */
		/** @{ */
		public:
			basic_iterator<IsConst>& operator=(const basic_iterator<IsConst>& origin)
			{
				_rear_node = origin._rear_node;
				_target_node = origin._target_node;
				_target_data = origin._target_data;
				return *this;
			}

			bool operator==(const basic_iterator<IsConst>& origin) const
			{
				return (origin._target_node == _target_node);
			}

			bool operator!=(const basic_iterator<IsConst>& origin) const
			{
				return (origin._target_node != _target_node);
			}

			bool operator==(const basic_iterator<!IsConst>& origin) const
			{
				return (origin._target_node == _target_node);
			}

			bool operator!=(const basic_iterator<!IsConst>& origin) const
			{
				return (origin._target_node != _target_node);
			}

			basic_iterator<IsConst>& operator++()
			{
				traverse();
				return *this;
			}

			basic_iterator<IsConst> operator++(int)
			{
				basic_iterator<IsConst> retval(*this);
				traverse();
				return retval;
			}

			template<bool _C = IsConst>
			std::enable_if_t<_C, reference>
			operator*() const
			{
				return _target_data->_data->value;
			}

			template<bool _C = IsConst>
			std::enable_if_t<!_C, reference>
			operator*()
			{
				return _target_data->value;
			}

			template<bool _C = IsConst>
			std::enable_if_t<_C, pointer>
			operator->() const
			{
				return &(_target_data->value);
			}

			template<bool _C = IsConst>
			std::enable_if_t<!_C, pointer>
			operator->()
			{
				return &(_target_data->value);
			}
		/** @} */
		};
		using iterator = basic_iterator<false>;
		using const_iterator = basic_iterator<true>;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		atomic_forward_list()
		:
			_head_node(new node())
		{

		}

		~atomic_forward_list()
		{
			/// @todo удаление всех node
		}
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
	private:
		const std::unique_ptr<node> _head_node;
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
	private:
		/**
		 * @brief insert
		 * @param base_node
		 * @param target_node could be displaced by an empty one
		 * @return
		 */
		iterator_tuple insert(node* base_node, std::unique_ptr<node>&& target_node)
		{
			if (!base_node || !target_node || target_node.get() == base_node || !target_node->_value_data) {
				return {};
			}

			/// @ocd
			target_node->_lead_node.store(nullptr);

			iterator_tuple retval{base_node, target_node->_value_data, nullptr};
			do {
				atomic_marked_ptr<node> spin_node(std::get<0>(retval)->_lead_node);
				if (!spin_node.exchange_mask_strong(atomic_forward_list::node_is_ready, atomic_forward_list::node_is_busy, true)) {
					continue;
				} else if (!spin_node) { /// < base_node прицеплена к nullptr
					if (spin_node.exchange_pointer_strong(marked_ptr<node>(target_node.get()), false)) {
						std::get<2>(retval) = target_node.release();
					}
				} else if (!spin_node->_value_data) { /// < использование незанятого узла
					if (atomic_compare_exchange_strong(&(spin_node->_value_data), &(spin_node->_value_data), target_node->_value_data)) {
						std::get<2>(retval) = spin_node.ptr();
					}
				} else { /// < вставка перед занятыми узлом
					target_node->_lead_node.store(marked_ptr<node>(spin_node.ptr()));
					if (!spin_node.exchange_pointer_strong(marked_ptr<node>(target_node.get()), false)) {
						target_node->_lead_node.store(nullptr);
					} else {
						std::get<2>(retval) = target_node.release();
					}
				}
			} while (!std::get<2>(retval));
			return retval;
		}

		/**
		 * @brief append
		 * @param base_node
		 * @param target_node could be displaced by an empty one
		 * @return
		 */
		iterator_tuple append(std::unique_ptr<node>&& target_node)
		{
			if (!target_node || !target_node->_value_data) {
				return {};
			}

			/// @ocd
			target_node->_lead_node.store(nullptr);

			std::tuple<node*, T, node*> retval{_head_node.get(), target_node->_value_data->value, nullptr};
			do {
				atomic_marked_ptr<node> spin_node(std::get<0>(retval)->_lead_node);
				if (!spin_node.load(atomic_forward_list::node_is_ready)) {
					continue;
				} else if (spin_node) { /// < ищем конец дальше
					std::get<0>(retval) = spin_node.ptr();
				} else if (!spin_node.exchange_mask_strong(atomic_forward_list::node_is_ready, atomic_forward_list::node_is_busy, true)) { /// < заблочим хвост
					/// @note Что-то изменилось - уходим на второй круг
					continue;
				} else if (!std::get<0>(retval)->_value_data) { /// < Сам узел и так свободен - его и займем
					if (atomic_compare_exchange_strong(&(spin_node->_value_data), &(spin_node->_value_data), target_node->_value_data)) {
						std::get<2>(retval) = spin_node.ptr();
					}
				} else if (spin_node.exchange_pointer_strong(target_node.get(), false)) {
					std::get<2>(retval) = target_node.release();
				}
			} while (!std::get<2>(retval));
			return retval;
		}

	public:
		/**
		 * @brief insert
		 * @param item
		 * @return iterator
		 */
		iterator insert(const const_iterator& i, const mapped_type& item) noexcept
		{
			return insert(i._target_node ? i._target_node : _head_node.get(), std::unique_ptr<node>(new node(item)));
		}

		/**
		 * @brief append
		 * @param item
		 * @return iterator
		 */
		iterator append(const mapped_type& item) noexcept
		{
			return append(std::unique_ptr<node>(new node(item)));
		}

		/**
		 * @brief prepend
		 * @param item
		 * @return iterator
		 */
		iterator prepend(const mapped_type& item)
		{
			return insert(std::unique_ptr<node>(new node(item)), _head_node.get());
		}

		/**
		 * @brief emplace
		 * @param item
		 * @return iterator
		 */
		template<typename ...TArgs>
		iterator emplace(const const_iterator& i, TArgs&& ...args)
		{
			return insert(i._target_node ? i._target_node : _head_node.get(), std::unique_ptr<node>(new node(T(std::forward<TArgs>(args)...))));
		}

		/**
		 * @brief emplace_back
		 * @param args
		 * @return iterator
		 */
		template<typename ...TArgs>
		iterator emplace_back(TArgs&& ...args)
		{
			return append(std::unique_ptr<node>(new node(std::forward<TArgs>(args)...)));
		}

		/**
		 * @brief emplace_front
		 * @param args
		 * @return iterator
		 */
		template<typename ...TArgs>
		iterator emplace_front(TArgs&& ...args)
		{
			return insert(_head_node.get(), std::unique_ptr<node>(new node(std::forward<TArgs>(args)...)));
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const_iterator i)
		{
			return atomic_compare_exchange_strong(&(i._target_data), &(i._target_data), std::shared_ptr<data>(nullptr)) ? ++i : i;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(const_iterator first, const_iterator last)
		{
			const_iterator retval;
			for (const_iterator i = first; i != last; i++) {
				retval = erase(i);
			}
			return retval;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(iterator i)
		{
			return atomic_compare_exchange_strong(&(i._target_data), &(i._target_data), std::shared_ptr<data>(nullptr)) ? ++i : i;
		}

		/**
		 * @brief erase
		 * @param i
		 * @return Iterator following the last removed element.
		 */
		iterator erase(iterator first, iterator last)
		{
			iterator retval;
			for (iterator i = first; i != last; i++) {
				retval = erase(i);
			}
			return retval;
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
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		iterator begin()
		{
			return iterator(_head_node.get());
		}

		const_iterator begin() const
		{
			return const_iterator(_head_node.get());
		}

		const_iterator cbegin() const
		{
			return const_iterator(_head_node.get());
		}

		iterator end()
		{
			return iterator();
		}

		const_iterator end() const
		{
			return const_iterator();
		}

		const_iterator cend() const
		{
			return const_iterator();
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
