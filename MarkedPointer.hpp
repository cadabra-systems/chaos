/**
 @file MarkedPointer.hpp
 @date 27.07.2018
 @copyright Daniil A Megrabyan
 @author Daniil A Megrabyan
*/

#ifndef Cadabra_Chaos_MarkedPointer_hpp
#define Cadabra_Chaos_MarkedPointer_hpp

#include <atomic>
#include <cstdint>
#include <cassert>
#include <optional>

namespace chaos {
	/**
	 * @class Marked pointer
	 * @brief
	 * On the modern architectures, the default data alignment is 4 (for 32bit) or 8 byte for 64bit.
	 * Therefore, the least 2 or 3 bits of the pointer is always zero and can
	 * be used as a bitfield to store logical flags. This trick is widely used in
	 * lock-free programming to operate with the pointer and its flags atomically.
	 * Template parameters:
	 * <T>: type of pointer
	 * <B>: bitmask of least unused bits
	 */
	template <typename T>
	class marked_ptr
	{
	/** @name Aliases */
	/** @{ */
	public:
		/**
		 * @brief type of value the class points to
		 */
		using value_type = T;

		/**
		 * @brief type of pointer
		 */
		using pointer_type = T*;
	/** @} */

	/** @name Classes */
	/** @{ */
	private:
		union pointer_cast
		{
			T* ptr;
			uintptr_t n;

			pointer_cast(T* pointer)
			:
				ptr(pointer)
			{

			}

			pointer_cast(uintptr_t i)
			:
				n(i)
			{

			}
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		/**
		 * @brief bitfield bitmask
		 */
		static constexpr const uintptr_t bitmask = 0b11;

		/**
		 * @brief pointer bitmask
		 */
		static constexpr const uintptr_t pointer_bitmask = ~bitmask;

	private:
		static uintptr_t to_int(value_type* pointer) noexcept
		{
			return pointer_cast(pointer).n;
		}

		static value_type* to_ptr(uintptr_t n) noexcept
		{
			return pointer_cast(n).ptr;
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		/**
		 * @brief Constructs null marked pointer. The flag is cleared.
		 */
		constexpr marked_ptr() noexcept
		:
			_ptr(nullptr)
		{

		}

		/**
		 * @brief Constructs null marked pointer. The flag is cleared.
		 */
		constexpr marked_ptr(std::nullptr_t) noexcept
		:
			_ptr(nullptr)
		{

		}

		/**
		 * @brief Constructs marked pointer with \p ptr value. The least bit(s) of \p ptr is the flag.
		 * @param ptr
		 */
		constexpr explicit marked_ptr(value_type* ptr) noexcept
		:
			_ptr(ptr)
		{

		}

		/**
		 * @brief Constructs marked pointer with \p ptr value and \p nMask flag.
		 * The \p mask argument defines the OR-bits
		 * @param ptr
		 * @param mask
		 */
		marked_ptr(value_type* ptr, int mask) noexcept
		:
			_ptr(ptr)
		{
			assert(mark() == 0);
			*this |= mask;
		}

		marked_ptr(marked_ptr const& other) noexcept = default;
		marked_ptr& operator=(marked_ptr const& other) noexcept = default;
		marked_ptr(marked_ptr&& other) noexcept = default;
		marked_ptr& operator=(marked_ptr&& other) noexcept = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		/**
		 * @brief _ptr pointer and its mark bits
		 */
		T* _ptr;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		/**
		 * @brief Returns the pointer without mark bits (real pointer) const version
		 * @return
		 */
		value_type* ptr() const noexcept
		{
			return to_ptr(to_int() & ~bitmask);
		}

		/**
		 * @brief Returns the pointer and bits together
		 * @return
		 */
		value_type* all() const noexcept
		{
			return _ptr;
		}

		/**
		 * @brief Returns the least bits of pointer according to \p Bitmask template argument of the class
		 * @return
		 */
		uintptr_t mark() const noexcept
		{
			return to_int() & bitmask;
		}

		/**
		 * @brief atomic<marked_ptr<T>> support
		 * @return
		 */
		T*& impl_ref() noexcept
		{
			return _ptr;
		}

	private:
		uintptr_t to_int() const noexcept
		{
			return to_int(_ptr);
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		/**
		 * @brief Analogue for \ref ptr
		 * @return
		 */
		value_type* operator->() const noexcept
		{
			return ptr();
		}

		/**
		 * @brief Assignment operator sets markup bits to zero
		 * @param pointer
		 * @return
		 */
		marked_ptr operator=(T* pointer) noexcept
		{
			_ptr = pointer;
			return *this;
		}

		/**
		 * @brief Set LSB bits as <tt>mark() | bits_count</tt>
		 * @param bits_count
		 * @return
		 */
		marked_ptr& operator|=(int bits_count) noexcept
		{
//			assert((bits_count & pointer_bitmask) == 0);
			_ptr = to_ptr(to_int() | bits_count);

			return *this;
		}

		/**
		 * @brief Set LSB bits as <tt>mark() & bits_count</tt>
		 * @param bits_count
		 * @return
		 */
		marked_ptr& operator&=(int bits_count) noexcept
		{
//			assert( (bits_count & pointer_bitmask) == 0 );
			_ptr = to_ptr( to_int() & (pointer_bitmask | bits_count));

			return *this;
		}

		/**
		 * @brief Set LSB bits as <tt>mark() ^ bits_count</tt>
		 * @param bits_count
		 * @return
		 */
		marked_ptr& operator^=(int bits_count) noexcept
		{
//			assert( (bits_count & pointer_bitmask) == 0 );
			_ptr = to_ptr(to_int() ^ bits_count);

			return *this;
		}

		/**
		 * @brief Returns <tt>p |= bits_count</tt>
		 * @param p
		 * @param bits_count
		 * @return
		 */
		friend marked_ptr operator|(marked_ptr p, int bits_count) noexcept
		{
			p |= bits_count;

			return p;
		}

		/**
		 * @brief Returns <tt>p |= bits_count</tt>
		 * @param bits_count
		 * @param p
		 * @return
		 */
		friend marked_ptr operator|(int bits_count, marked_ptr p) noexcept
		{
			p |= bits_count;

			return p;
		}

		/**
		 * @brief Returns <tt>p &= bits_count</tt>
		 * @param p
		 * @param bits_count
		 * @return
		 */
		friend marked_ptr operator&(marked_ptr p, int bits_count) noexcept
		{
			p &= bits_count;

			return p;
		}

		/**
		 * @brief Returns <tt>p &= bits_count</tt>
		 * @param bits_count
		 * @param p
		 * @return
		 */
		friend marked_ptr operator&(int bits_count, marked_ptr p) noexcept
		{
			p &= bits_count;

			return p;
		}

		/**
		 * @brief Returns <tt>p ^= bits_count</tt>
		 * @param p
		 * @param bits_count
		 * @return
		 */
		friend marked_ptr operator^(marked_ptr p, int bits_count) noexcept
		{
			p ^= bits_count;

			return p;
		}

		/**
		 * @brief Returns <tt>p ^= bits_count</tt>
		 * @param bits_count
		 * @param p
		 * @return
		 */
		friend marked_ptr operator^(int bits_count, marked_ptr p) noexcept
		{
			p ^= bits_count;

			return p;
		}

		/**
		 * @brief Inverts LSBs of pointer \p p
		 * @param p
		 * @return
		 */
		friend marked_ptr operator~(marked_ptr p) noexcept
		{
			return p ^ marked_ptr::bitmask;
		}

		/**
		 * @brief Comparing two marked pointer including its mark bits
		 * @param p1
		 * @param p2
		 * @return
		 */
		friend bool operator==(marked_ptr p1, marked_ptr p2) noexcept
		{
			return p1.all() == p2.all();
		}

		/**
		 * @brief Comparing marked pointer and raw pointer, mark bits of \p p1 is ignored
		 * @param p1
		 * @param p2
		 * @return
		 */
		friend bool operator==(marked_ptr p1, value_type const * p2) noexcept
		{
			return p1.ptr() == p2;
		}

		/**
		 * @brief Comparing marked pointer and null pointer, mark bits of \p p1 is ignored
		 * @param p1
		 * @return
		 */
		friend bool operator==(marked_ptr p1, std::nullptr_t) noexcept
		{
			return p1.ptr() == nullptr;
		}

		/**
		 * @brief Comparing marked pointer and raw pointer, mark bits of \p p2 is ignored
		 * @param p1
		 * @param p2
		 * @return
		 */
		friend bool operator==(value_type const * p1, marked_ptr p2) noexcept
		{
			return p1 == p2.ptr();
		}

		/**
		 * @brief Comparing marked pointer and null pointer, mark bits of \p p2 is ignored
		 * @param p2
		 * @return
		 */
		friend bool operator==(std::nullptr_t, marked_ptr p2) noexcept
		{
			return nullptr == p2.ptr();
		}

		/**
		 * @brief Comparing two marked pointer including its mark bits
		 * @param p1
		 * @param p2
		 * @return
		 */
		friend bool operator!=(marked_ptr p1, marked_ptr p2) noexcept
		{
			return p1.all() != p2.all();
		}

		/**
		 * @brief Comparing marked pointer and raw pointer, mark bits of \p p1 is ignored
		 * @param p1
		 * @param p2
		 * @return
		 */
		friend bool operator!=(marked_ptr p1, value_type const * p2) noexcept
		{
			return p1.ptr() != p2;
		}

		/**
		 * @brief Comparing marked pointer and null pointer, mark bits of \p p1 is ignored
		 * @param p1
		 * @return
		 */
		friend bool operator!=(marked_ptr p1, std::nullptr_t) noexcept
		{
			return p1.ptr() != nullptr;
		}

		/**
		 * @brief Comparing marked pointer and raw pointer, mark bits of \p p2 is ignored
		 * @param p1
		 * @param p2
		 * @return
		 */
		friend bool operator!=(value_type const * p1, marked_ptr p2) noexcept
		{
			return p1 != p2.ptr();
		}

		/**
		 * @brief Comparing marked pointer and std::nullptr_t
		 * @param p2
		 * @return
		 */
		friend bool operator!=(std::nullptr_t, marked_ptr p2) noexcept
		{
			return nullptr != p2.ptr();
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void dispose()
		{
			delete ptr();
			_ptr = nullptr;
		}

		void reset()
		{
			_ptr = nullptr;
		}

		T* release(bool marked = true)
		{
			T* retval(marked ? all() : ptr());
			_ptr = nullptr;

			return retval;
		}
	/** @} */
	};

	template <typename T>
	class atomic_marked_ptr
	{
	/** @name Constructors */
	/** @{ */
	public:
		atomic_marked_ptr(std::atomic<marked_ptr<T>>& source)
		:
			_source(source),
			_current(nullptr)
		{

		}

		~atomic_marked_ptr()
		{
			if (_backup_mask.has_value() && _backup_mask.value() != _current.mark()) {
				_source.compare_exchange_strong(_current, _current & _backup_mask.value());
			}
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::memory_order _default_memory_order = std::memory_order::memory_order_seq_cst;

		std::atomic<marked_ptr<T>>& _source;
		marked_ptr<T> _current;

		std::optional<uintptr_t> _backup_mask;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void load()
		{
			_current = _source.load(_default_memory_order);
		}

		bool load(uintptr_t expected_mask)
		{
			_current = _source.load(_default_memory_order);
			if (_current.mark() == expected_mask) {
				return true;
			}
			_current.reset();
			return false;
		}

		void store(const marked_ptr<T>& value, bool keep_backup = false)
		{
			_current = value;
			_source.store(_source, _default_memory_order);
			if (!keep_backup) {
				_backup_mask.reset();
			}
		}

		bool exchange_pointer_strong(const marked_ptr<T>& value, bool keep_backup = false)
		{
			if (!_current.all()) {
				_current = _source.load(_default_memory_order);
			}
			if (!_source.compare_exchange_strong(_current, value)) {
				return false;
			}
			_current = value;
			if (!keep_backup) {
				_backup_mask.reset();
			}
			return true;
		}

		bool exchange_mask_strong(uintptr_t expected_mask, uintptr_t desired_mask, bool backup = true)
		{
			if (!_current.all()) {
				_current = _source.load(_default_memory_order);
			}
			marked_ptr<T> expected_pointer(_current.ptr(), expected_mask);
			marked_ptr<T> desired_pointer(_current.ptr(), desired_mask);
			if (!_source.compare_exchange_strong(expected_pointer, desired_pointer)) {
				return false;
			} else if (backup) {
				_backup_mask.emplace(expected_mask);
			}
			return true;
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		operator bool() const noexcept
		{
			return _current.ptr() != nullptr;
		}

		bool operator!() const noexcept
		{
			return _current.ptr() == nullptr;
		}

		typename marked_ptr<T>::value_type* operator->() const noexcept
		{
			return _current.operator->();
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		typename marked_ptr<T>::value_type* ptr() const noexcept
		{
			return _current.ptr();
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set_backup_mask(uintptr_t value)
		{
			_backup_mask.emplace(value);
		}

		void set_backup_mask(std::nullptr_t)
		{
			_backup_mask.reset();
		}

		void set_default_memory_order(std::memory_order value)
		{
			_default_memory_order = value;
		}
	/** @} */
	};

	template<typename T, int B = 0b00, typename ...TArgs>
	marked_ptr<T> make_marked(TArgs&& ...args)
	{
		return marked_ptr<T>(new T(std::forward<TArgs>(args)... ), B);
	}
}

#endif
