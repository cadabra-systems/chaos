//
//  Any.hpp
//  Abra
//
//  Created by Daniil A Megrabyan on 20.05.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Any_hpp
#define Chaos_Any_hpp

#include "Flex.hpp"

#include <set>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <typeinfo>

namespace chaos {
	template <typename T>
	void* any_pointer(T& anything)
	{
		return static_cast<void*>(&anything);
	}

	template <typename T>
	const void* any_pointer(const T& anything)
	{
		return static_cast<const void*>(&anything);
	}

	template <typename T>
	std::size_t any_size(const T&)
	{
		return sizeof(T);
	}

	template <typename T>
	chaos::flex any_flex(const T& value)
	{
		return value;
	}

	inline void* any_pointer(std::string& value)
	{
		/// @xxx Был .begin(), но это 100% некорректно
		return value.data();
	}

	inline const void* any_pointer(const std::string& value)
	{
		/// @xxx Был .begin(), но это 100% некорректно
		return value.data();
	}

	inline std::size_t any_size(const std::string& value)
	{
		return value.length();
	}

	class any
	{
	/** @name Aliases */
	/** @{ */
	/** @} */

	/** @name Classes */
	/** @{ */
	private:
		struct holder
		{
			virtual ~holder() = default;
			virtual std::shared_ptr<holder> clone(bool data) const = 0;
			virtual const std::type_info& type_info() const = 0;
			virtual flex flex() const = 0;
			virtual std::size_t size() const = 0;
			virtual std::size_t count() const = 0;
			virtual void* ptr() = 0;
			virtual const void* ptr() const = 0;
			virtual void* operator[](std::size_t index) = 0;
			virtual const void* operator[](std::size_t index) const = 0;
			virtual bool operator==(const holder& rhs) const = 0;
			virtual bool operator!=(const holder& rhs) const = 0;
			virtual bool operator<(const holder& rhs) const = 0;
			virtual bool operator>(const holder& rhs) const = 0;
			virtual bool operator>=(const holder& rhs) const = 0;
			virtual bool operator<=(const holder& rhs) const = 0;

			virtual std::shared_ptr<holder> operator|(const holder& rhs) const = 0;
			virtual std::shared_ptr<holder> operator&(const holder& rhs) const = 0;
		};

		template<typename T>
		struct variable : holder
		{
		/** @name Constructors */
		/** @{ */
			variable(const T& value)
			:
				_value(value)
			{

			}

			variable(T&& value)
			:
				_value(std::move(value))
			{

			}

			variable()
			{

			}
		/** @} */

		/** @name Properties */
		/** @{ */
			T _value;
		/** @} */

		/** @name Factories */
		/** @{ */
			virtual std::shared_ptr<holder> clone(bool data) const override
			{
				return data ? std::make_shared<variable<T>>(_value) : std::make_shared<variable<T>>();
			}
		/** @} */

		/** @name Getters */
		/** @{ */
			const std::type_info& type_info() const override
			{
				return typeid(T);
			}

			chaos::flex flex() const override
			{
				return any_flex(_value);
			}

			std::size_t size() const override
			{
				return any_size(_value);
			}

			std::size_t count() const override
			{
				return 1;
			}

			void* ptr() override
			{
				return any_pointer(_value);
			}

			const void* ptr() const override
			{
				return any_pointer(_value);
			}
		/** @} */

		/** @name Operators */
		/** @{ */
			virtual void* operator[](std::size_t index) override
			{
				return index == 0 ? &_value : nullptr;
			}

			virtual const void* operator[](std::size_t index) const override
			{
				return index == 0 ? &_value : nullptr;
			}

			bool operator==(const holder& rhs) const override
			{
				return (type_info() == rhs.type_info()) ? rhs.count() == 1 && *static_cast<const T*>(rhs[0]) == _value : false;
			}

			bool operator!=(const holder& rhs) const override
			{
				return (type_info() == rhs.type_info()) ? rhs.count() != 1 || *static_cast<const T*>(rhs[0]) != _value : false;
			}

			bool operator<(const holder& rhs) const override
			{
				return (type_info() == rhs.type_info()) ? rhs.count() > 1 || *static_cast<const T*>(rhs[0]) > _value : false;
			}

			bool operator>(const holder& rhs) const override
			{
				return (type_info() == rhs.type_info()) ? rhs.count() < 1 || *static_cast<const T*>(rhs[0]) < _value : false;
			}

			bool operator<=(const holder& rhs) const override
			{
				return (type_info() == rhs.type_info()) ? rhs.count() > 1 || (rhs.count() == 1 && *static_cast<const T*>(rhs[0]) >= _value) : false;
			}

			bool operator>=(const holder& rhs) const override
			{
				return (type_info() == rhs.type_info()) ? rhs.count() < 1 || (rhs.count() == 1 && *static_cast<const T*>(rhs[0]) <= _value) : false;
			}

			virtual std::shared_ptr<holder> operator|(const holder& rhs) const override
			{
				std::set<T> intersection_set{_value};
				for (std::size_t i = 0; i < rhs.count(); i++) {
					intersection_set.emplace(*static_cast<const T*>(rhs[i]));
				}
				return std::make_shared<collection<T>>(intersection_set.begin(), intersection_set.end());
			}

			virtual std::shared_ptr<holder> operator&(const holder& rhs) const override
			{
				for (std::size_t i = 0; i < rhs.count(); i++) {
					const T& r(*static_cast<const T*>(rhs[i]));
					const T& l(_value);
					if (l == r) {
					//if (*static_cast<const T*>(rhs[i]) == _value) {
						return std::make_shared<variable<T>>(_value);
					}
				}
				return nullptr;
			}
		/** @} */
		};

		template<typename T>
		struct collection : holder
		{
		/** @name Constructors */
		/** @{ */
			collection()
			:
				_value(nullptr),
				_count(0)
			{

			}

			collection(const T& value)
			:
				_value(new T[1]),
				_count(1)
			{
				_value[0] = value;
			}

			collection(const T*& value, std::size_t count)
			:
				_value(nullptr),
				_count(0)
			{
				/// @todo
			}

			collection(const std::initializer_list<T>& list)
			:
				_value(new T[list.size()]),
				_count(0)
			{
				for (const T& v : list) {
					_value[_count++] = v;
				}
			}

			template <typename I>
			collection(I i, I end)
			:
				_value(new T[std::distance(i, end)]),
				_count(0)
			{
				for (; i != end; ++i) {
					_value[_count++] = *i;
				}
			}

			collection(T&& origin)
			:
				_value(std::move(origin._value)),
				_count(origin._count)
			{
				origin._count = 0;
			}

			~collection()
			{
				if (_value) {
					delete[] _value;
				}
			}
		/** @} */

		/** @name Properties */
		/** @{ */
			T* _value;
			std::size_t _count;
		/** @} */

		/** @name Factories */
		/** @{ */
			virtual std::shared_ptr<holder> clone(bool data) const override
			{
				return /*data ? std::make_shared<collection<T>>(std::cref(_value), _count) : */std::make_shared<collection<T>>();
			}
		/** @} */

		/** @name Getters */
		/** @{ */
			const std::type_info& type_info() const override
			{
				return typeid(T);
			}

			chaos::flex flex() const override
			{
				chaos::flex retval;
				std::set<std::size_t>::iterator i2;
				for (std::size_t i1 = 0; i1 < _count; i1++) {
					retval.push_back(any_flex(_value[i1]));
				}
				return retval;
			}

			std::size_t size() const override
			{
				std::size_t retval(0);
				/// @todo sumup
//				std::for_each(std::begin(_value), std::end(_value), [&retval](const T*& v) { retval += any_size(*v); });
				return retval;
			}

			std::size_t count() const override
			{
				return _count;
			}

			void* ptr() override
			{
				return any_pointer(_value);
			}

			const void* ptr() const override
			{
				return any_pointer(_value);
			}
		/** @} */

		/** @name Operators */
		/** @{ */
			/**
			 * @brief operator []
			 * @param index
			 * @return
			 */
			virtual void* operator[](std::size_t index) override
			{
				return &_value[index];
			}

			/**
			 * @brief operator []
			 * @param index
			 * @return
			 */
			virtual const void* operator[](std::size_t index) const override
			{
				return &_value[index];
			}

			/**
			 * @brief operator ==
			 * @todo
			 * @param rhs
			 * @return
			 */
			bool operator==(const holder& rhs) const override
			{
				if (type_info() != rhs.type_info() || rhs.count() != count()) {
					return false;
				}

				std::size_t c(0);
				std::set<std::size_t> search_set;
				std::generate_n(
								std::inserter
								(
									search_set,
									search_set.begin()
								),
								_count,
								[&c]()
								{
									return c++;
								}
				);

				std::set<std::size_t>::iterator i2;
				for (std::size_t i1 = 0; i1 < _count; i1++) {
					for (i2 = search_set.begin(); i2 != search_set.end(); i2++) {
						if ((static_cast<const T*>(rhs.ptr())[*i2]) == _value[i1]) {
							search_set.erase(i2);
							break;
						}
					}
					if (i2 == search_set.end()) {
						return false;
					}
				}
				return search_set.empty();
			}

			/**
			 * @brief operator !=
			 * @todo
			 * @param rhs
			 * @return
			 */
			bool operator!=(const holder& rhs) const override
			{
				return operator==(rhs) == false;
			}

			/**
			 * @brief operator <
			 * @todo
			 * @param rhs
			 * @return
			 */
			bool operator<(const holder& rhs) const override
			{
				return false; /// (type_info() == rhs.type_info()) ? rhs.count() > count() || *static_cast<const T*>(rhs.ptr()) > _value.get() : false;
			}

			/**
			 * @brief operator >
			 * @todo
			 * @param rhs
			 * @return
			 */
			bool operator>(const holder& rhs) const override
			{
				return false; /// (type_info() == rhs.type_info()) ? rhs.count() < count() || *static_cast<const T*>(rhs.ptr()) < _value.get() : false;
			}

			/**
			 * @brief operator <=
			 * @todo
			 * @param rhs
			 * @return
			 */
			bool operator<=(const holder& rhs) const override
			{
				return false; /// (type_info() == rhs.type_info()) ? rhs.count() < count() || (rhs.count() == count() && *static_cast<const T*>(rhs.ptr()) >= _value.get()) : false;
			}

			/**
			 * @brief operator >=
			 * @todo
			 * @param rhs
			 * @return
			 */
			bool operator>=(const holder& rhs) const override
			{
				return false; /// (type_info() == rhs.type_info()) ? rhs.count() < count() || (rhs.count() == count() && *static_cast<const T*>(rhs.ptr()) <= _value.get()) : false;
			}

			virtual std::shared_ptr<holder> operator|(const holder& rhs) const override
			{
				std::set<T> conjunction_set;
				for (std::size_t i = 0; i < rhs.count(); i++) {
					conjunction_set.emplace(*static_cast<const T*>(rhs[i]));
				}
				for (std::size_t i = 0; i < _count; i++) {
					conjunction_set.emplace(_value[i]);
				}
				return std::make_shared<collection<T>>(conjunction_set.begin(), conjunction_set.end());
			}

			virtual std::shared_ptr<holder> operator&(const holder& rhs) const override
			{
				std::set<T> intersection_set;
				for (std::size_t i1 = 0; i1 < rhs.count(); i1++) {
					for (std::size_t i2 = 0; i2 < _count; i2++) {
						if (*static_cast<const T*>(rhs[i1]) == _value[i2]) {
							intersection_set.emplace(_value[i2]);
						}
					}
				}
				return std::make_shared<collection<T>>(intersection_set.begin(), intersection_set.end());
			}
		/** @} */
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		template<typename T>
		any(const T& value)
		:
			_holder(new variable<T>(value))
		{

		}

		template <typename T>
		any(T&& value)
		:
			_holder(new variable<typename std::decay<T>::type>(std::forward<T>(value)))
		{

		}

		template <typename T>
		any(const std::initializer_list<T>& value)
		:
			_holder(new collection<typename std::decay<T>::type>(value))
		{

		}

		any()
		:
			_holder(nullptr)
		{

		}

		any(std::nullptr_t)
		:
			_holder(nullptr)
		{

		}

		any(any&& origin)
		:
			_holder(std::move(origin._holder))
		{

		}

		any(const any& origin)
		:
			_holder(origin._holder)
		{

		}

		any(any& origin)
		:
			_holder(origin._holder)
		{

		}

		~any() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<holder> _holder;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		any& operator=(any&& value)
		{
			_holder = std::move(value._holder);
			return *this;
		}

		any& operator=(const any& value)
		{
			_holder = value._holder;
			return *this;
		}

		any& operator=(std::nullptr_t)
		{
			_holder.reset();
			return *this;
		}

		bool operator!() const
		{
			return (nullptr == _holder);
		}

		bool operator==(const any& rhs) const
		{
			if (nullptr == _holder) {
				return (nullptr == rhs._holder);
			} else if (nullptr == rhs._holder) {
				return false;
			}
			return (*_holder.get()) == (*rhs._holder.get());
		}

		bool operator<(const any& rhs) const
		{
			if (nullptr == _holder ) {
				return (nullptr != rhs._holder);
			} else if (nullptr == rhs._holder) {
				return false;
			}
			return (*_holder.get()) < (*rhs._holder.get());
		}

		bool operator<=(const any& rhs) const
		{
			if (nullptr == _holder) {
				return true;
			} else if (nullptr == rhs._holder) {
				return false;
			}
			return (*_holder.get()) <= (*rhs._holder.get());
		}

		bool operator>(const any& rhs) const
		{
			if (nullptr == rhs._holder) {
				return (nullptr != _holder);
			} else if (nullptr == _holder ) {
				return false;
			}
			return (*_holder.get()) > (*rhs._holder.get());
		}

		bool operator>=(const any& rhs) const
		{
			if (nullptr == rhs._holder) {
				return true;
			} else if (nullptr == _holder) {
				return false;
			}
			return (*_holder.get()) >= (*rhs._holder.get());
		}

		template<typename U>
		any& operator=(const U& value)
		{
			if (nullptr == _holder) {
				_holder.reset(new variable<U>(value));
			} else if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			static_cast<variable<U>*>(_holder.get())->_value = value;
			return *this;
		}

		template<typename U>
		operator const U&() const
		{
			if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			return static_cast<variable<U>*>(_holder.get())->_value;
		}

		template<typename U>
		operator U&()
		{
			if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			return static_cast<variable<U>*>(_holder.get())->_value;
		}

		template<typename U>
		operator U()
		{
			if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			return static_cast<variable<U>*>(_holder.get())->_value;
		}

		/**
		 * @brief union
		 * @param rhs
		 * @return
		 */
		any operator|(const any& rhs) const
		{
			any retval;
			if (!_holder) {
				if (!rhs._holder) {
					retval._holder = nullptr;
				} else {
					retval._holder = rhs._holder->clone(true);
				}
			} else if (!rhs._holder) {
				if (!_holder) {
					retval._holder = nullptr;
				} else {
					retval._holder = _holder->clone(true);
				}
			} else if (_holder->type_info() != rhs._holder->type_info()) {
				retval._holder = _holder->clone(false);
			} else if (std::min(_holder->count(), rhs._holder->count()) == 0) {
				/// @todo Пустая коллекция
				return any();
			} else {
				retval._holder = (*_holder) | (*rhs._holder);
			}
			return retval;
		}

		/**
		 * @brief intersection
		 * @todo
		 * @param rhs
		 * @return
		 */
		any operator&(const any& rhs) const
		{
			any retval;
			if (!_holder) {
				if (!rhs._holder) {
					retval._holder = nullptr;
				} else {
					retval._holder = rhs._holder->clone(false);
				}
			} else if (!rhs._holder) {
				if (!_holder) {
					retval._holder = nullptr;
				} else {
					retval._holder = _holder->clone(false);
				}
			} else if (_holder->type_info() != rhs._holder->type_info()) {
				retval._holder = _holder->clone(false);
			} else {
				/// @note Если count у одного holder == 0, то полуичм пустую коллекцию, а не any(nullptr)
				retval._holder = (*_holder) & (*rhs._holder);
			}
			return retval;
		}

		/**
		 * @brief mised in both
		 * @todo
		 * @param rhs
		 * @return
		 */
		any operator^(const any& rhs) const
		{
			return any();
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void reset()
		{
			_holder.reset();
		}

		template<typename U>
		void reset(const U& value)
		{
			_holder.reset(new variable<U>(value));
		}

		template<typename U>
		void reset(const U&& value)
		{
			_holder.reset(new variable<U>(std::move(value)));
		}

		template <typename I>
		typename std::enable_if<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<I>::iterator_category>, void>::type
		reset(I first, I last)
		{
			using U = typename std::iterator_traits<I>::value_type;
			_holder.reset(new collection<U>(std::forward<I>(first), std::forward<I>(last)));
		}

		template<typename U>
		void reset(std::set<U>& set) const
		{
			set.clear();
			if (!_holder) {
				return ;
			} else if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			for (std::size_t i = 0, length = _holder->count(); i < length; i++) {
				set.emplace(static_cast<variable<U>*>(_holder.get() + (_holder->size() * i))->_value);
			}
		}

		template<typename U>
		void reset(std::list<U>& list) const
		{
			list.clear();
			if (!_holder) {
				return ;
			} else if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			for (std::size_t i = 0, length = _holder->count(); i < length; i++) {
				list.emplace_back(static_cast<variable<U>*>(_holder.get() + (_holder->size() * i))->_value);
			}
		}

		template<typename U>
		void reset(std::vector<U>& vector) const
		{
			vector.clear();
			if (!_holder) {
				return ;
			} else if (typeid(U) != _holder->type_info()) {
				throw std::bad_cast();
			}
			for (std::size_t i = 0, length = _holder->count(); i < length; i++) {
				vector.emplace_back(static_cast<variable<U>*>(_holder.get() + (_holder->size() * i))->_value);
			}
		}
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		chaos::flex flex() const
		{
			return !_holder ? chaos::flex{} : _holder->flex();
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		void* ptr()
		{
			return !_holder ? nullptr : _holder->ptr();
		}

		std::size_t size() const
		{
			return !_holder ? 0: _holder->size();
		}

		const std::type_info& type_info() const
		{
			return !_holder ? typeid(std::nullptr_t) : _holder->type_info();
		}

		bool is_null() const
		{
			return !_holder;
		}
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const any null;
	/** @} */
	};
}

#endif
