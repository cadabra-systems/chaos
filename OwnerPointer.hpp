/**
 @file OwnerPointer.hpp
 @date 27.07.2018
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Cadabra_Chaos_OwnerPointer_hpp
#define Cadabra_Chaos_OwnerPointer_hpp

#include <memory>
#include <utility>

namespace chaos {
	template<typename T>
	class owner_ptr
	{
	/** @name Aliases*/
	/** @{ */
	public:
		using deleter_type = void (*)(T*);
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		owner_ptr(T* pointer = nullptr)
		:
			_pointer(pointer),
			_deleter(std::default_delete<T*>())
		{

		}

		owner_ptr(T* pointer, deleter_type deleter)
		:
			_pointer(pointer),
			_deleter(deleter)
		{

		}

		owner_ptr(owner_ptr<T>&& origin)
		:
			_pointer(origin._pointer),
			_deleter(origin._deleter)
		{
			origin._pointer = nullptr;
			origin._deleter = nullptr;
		}

		owner_ptr(const owner_ptr<T>& pointer) = delete;

		~owner_ptr()
		{
			reset(nullptr);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		T* _pointer;
		deleter_type _deleter;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		owner_ptr<T>& operator=(owner_ptr<T>&& origin)
		{
			reset();
			_pointer = origin._pointer;
			_deleter = origin._deleter;
			return *this;
		}

		owner_ptr<T>& operator=(const owner_ptr<T>& origin) = delete;

		template<typename U = T>
		typename std::enable_if<!std::is_void<U>::value, U&>::type
		operator*() const
		{
			return *_pointer;
		}

		T* operator->() const
		{
			static_assert(!std::is_array<T>::value, "chaos::owner_ptr<T>::operator-> is only valid when T is not an array type.");
			return _pointer;
		}

		bool operator!() const
		{
			return nullptr == _pointer;
		}

		operator bool() const
		{
			return nullptr != _pointer;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		T* take()
		{
			T* retval(_pointer);
			_pointer = nullptr;
			return retval;
		}

		T* get() const
		{
			return _pointer;
		}

		T* get_pointer() const
		{
			return _pointer;
		}

		deleter_type get_deleter() const
		{
			return _deleter;
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void reset(T* pointer = nullptr, deleter_type deleter = nullptr)
		{
			if (_pointer && _deleter) {
				_deleter(_pointer);
			}
			_pointer = pointer;
			_deleter = deleter;
		}
	/** @} */

	/** @name Friends */
	/** @{ */
		friend void swap(owner_ptr<T>& lhs, owner_ptr<T>& rhs)
		{
			std::swap(lhs._pointer, rhs._pointer);
			std::swap(lhs._deleter, rhs._deleter);
		}
	/** @} */
	};

	template<typename T, typename ...TArgs>
	chaos::owner_ptr<T> make_owner(TArgs&& ...args)
	{
		return owner_ptr<T>(new T(std::forward<TArgs>(args)...));
	}

	template<typename T, typename ...TArgs>
	chaos::owner_ptr<T> make_owner(TArgs&& ...args, void (*deleter)(T*))
	{
		return owner_ptr<T>(new T(std::forward<TArgs>(args)...), deleter);
	}
}

#endif
