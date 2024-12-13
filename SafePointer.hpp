/**
 @file SafePointer.hpp
 @date 27.07.2018
 @copyright Daniil A Megrabyan
 @author Daniil A Megrabyan
 @url https://habr.com/ru/post/328348/
*/

#ifndef Cadabra_Chaos_SafePointer_hpp
#define Cadabra_Chaos_SafePointer_hpp

#include <cassert>
#include <memory>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

// Autodetect C++14
#if (__cplusplus >= 201402L || _MSC_VER >= 1900)
#define SHARED_MTX
#include <shared_mutex>
#endif

namespace chaos {
	template<typename T, typename M = std::recursive_mutex, typename x_lock_t = std::unique_lock<M>, typename s_lock_t = std::unique_lock<M>>
//	std::shared_lock<std::shared_timed_mutex>, when M = std::shared_timed_mutex
	class safe_ptr
	{
	protected:
		std::shared_ptr<T> _ptr; /// < const shared_ptr<T> or std::propagate_const<std::shared_ptr<T>> ptr;
		std::shared_ptr<M> _mtx_ptr;

		template<typename req_lock>
		class auto_lock_t
		{
		private:
			T* const _ptr;
			req_lock _lock;

		public:
			auto_lock_t(auto_lock_t&& o)
			:
				_ptr(std::move(o._ptr)),
				_lock(std::move(o._lock))
			{

			}

			auto_lock_t(T* const _ptr, M& _mtx)
			:
				_ptr(_ptr),
				_lock(_mtx)
			{

			}

			T* operator->()
			{
				return _ptr;
			}

			const T* operator->() const
			{
				return _ptr;
			}
		};

		template<typename req_lock>
		class auto_lock_obj_t
		{
		private:
			T* const _ptr;
			req_lock _lock;

		public:
			auto_lock_obj_t(auto_lock_obj_t&& o)
			:
				_ptr(std::move(o._ptr)),
				_lock(std::move(o._lock))
			{

			}

			auto_lock_obj_t(T* const ptr, M& mtx)
			:
				_ptr(ptr),
				_lock(mtx)
			{
			}

			template<typename arg_t>
			auto operator[] (arg_t &&arg) -> decltype((*_ptr)[arg])
			{
				return (*_ptr)[arg];
			}
		};

		struct no_lock_t
		{
			no_lock_t(no_lock_t &&)
			{

			}

			template<typename sometype> no_lock_t(sometype&)
			{

			}
		};

		using auto_nolock_t = auto_lock_obj_t<no_lock_t>;

		T* get_obj_ptr() const
		{
			return _ptr.get();
		}

		M* get_mtx_ptr() const
		{
			return _mtx_ptr.get();
		}

		template<typename... Args>
		void lock_shared() const
		{
			get_mtx_ptr()->lock_shared();
		}

		template<typename... Args>
		void unlock_shared() const
		{
			get_mtx_ptr()->unlock_shared();
		}

		void lock() const
		{
			get_mtx_ptr()->lock();
		}

		void unlock() const
		{
			get_mtx_ptr()->unlock();
		}

		friend struct link_safe_ptrs;
		template<typename, typename, typename, typename>
		friend class safe_obj;

		template<typename some_type>
		friend struct xlocked_safe_ptr;

		template<typename some_type>
		friend struct slocked_safe_ptr;

		template<typename, typename, size_t, size_t>
		friend class lock_timed_transaction;

		template<size_t, typename, size_t, size_t>
		friend class lock_timed_any;

#if (_WIN32 && _MSC_VER < 1900 || __clang__)
		template<class Type>
		friend class std::lock_guard;  // MSVS2013 or Clang 4.0
#else
		template<class... Types>
		friend class std::lock_guard;  // C++17 or MSVS2015
#endif
#ifdef SHARED_MTX
		template<typename Type>
		friend class std::shared_lock;  // C++14
#endif

		template<typename Type>
		friend struct std::hash;

		/// @brief This allow pointer_cast functions to share the reference counter between different shared_ptr types
		template<typename Z, typename, typename, typename>
		friend class safe_ptr;

	public:
		template<typename... Args>
		safe_ptr(Args... args)
		:
			_ptr(std::make_shared<T>(args...)),
			_mtx_ptr(std::make_shared<M>())
		{

		}

		safe_ptr(std::nullptr_t)
		:
			_ptr(nullptr),
			_mtx_ptr(nullptr)
		{

		}

		safe_ptr()
		:
			_ptr(nullptr),
			_mtx_ptr(nullptr)
		{

		}

		safe_ptr(std::shared_ptr<T> smart_pointer)
		:
			_ptr(smart_pointer),
			_mtx_ptr(std::make_shared<M>())
		{

		}

		safe_ptr(T* raw_pointer)
		:
			_ptr(raw_pointer),
			_mtx_ptr(std::make_shared<M>())
		{

		}

		template <typename Z, class = typename std::enable_if<std::is_convertible<std::add_pointer_t<Z>, std::add_pointer_t<T>>::value>::type>
		safe_ptr(safe_ptr<Z> safe_pointer)
		:
			_ptr(safe_pointer._ptr),
			_mtx_ptr(safe_pointer._mtx_ptr)
		{

		}

		~safe_ptr() = default;

/*
		auto_lock_t<x_lock_t> operator->()
		{
			return auto_lock_t<x_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		auto_lock_obj_t<x_lock_t> operator*()
		{
			return auto_lock_obj_t<x_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		const auto_lock_t<s_lock_t> operator->() const
		{
			return auto_lock_t<s_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		const auto_lock_obj_t<s_lock_t> operator*() const
		{
			return auto_lock_obj_t<s_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}
*/

		auto_lock_t<s_lock_t> operator->() const
		{
			return auto_lock_t<s_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		auto_lock_obj_t<s_lock_t> operator*() const
		{
			return auto_lock_obj_t<s_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		operator bool() const
		{
			return !!_ptr;
		}

		bool operator !() const
		{
			return !_ptr;
		}

		template <typename Z, class = typename std::enable_if<std::is_convertible<std::add_pointer_t<Z>, std::add_pointer_t<T>>::value>::type>
		safe_ptr<T>& operator=(safe_ptr<Z> safe_pointer)
		{
			_ptr = safe_pointer._ptr;
			_mtx_ptr = safe_pointer._mtx_ptr;

			return *this;
		}

		bool operator<(const safe_ptr<T>& rhs) const noexcept
		{
			return _ptr < rhs._ptr;
		}

		long use_count() const
		{
			return _ptr.use_count();
		}

		void reset()
		{
			_ptr.reset();
			_mtx_ptr.reset();
		}

		template <typename Z, class = typename std::enable_if<std::is_convertible<std::add_pointer_t<Z>, std::add_pointer_t<T>>::value>::type>
		void reset(Z* raw_pointer)
		{
			_ptr.reset(raw_pointer);
			_mtx_ptr.reset((nullptr == raw_pointer) ? nullptr : new M());
		}

		using mtx_t = M;
		using obj_t = T;
		using xlock_t = x_lock_t;
		using slock_t = s_lock_t;
	};

	template<typename T> using default_safe_ptr = safe_ptr<T, std::recursive_mutex, std::unique_lock<std::recursive_mutex>, std::unique_lock<std::recursive_mutex>>;
#ifdef SHARED_MTX // C++14
	template<typename T> using shared_mutex_safe_ptr = safe_ptr< T, std::shared_timed_mutex, std::unique_lock<std::shared_timed_mutex>, std::shared_lock<std::shared_timed_mutex> >;
#endif

	template<typename T, typename M = std::recursive_mutex, typename x_lock_t = std::unique_lock<M>, typename s_lock_t = std::unique_lock<M>>
	class safe_obj
	{
	protected:
		T _obj;
		mutable M _mtx;

		T* get_obj_ptr() const
		{
			return const_cast<T*>(&_obj);
		}

		M * get_mtx_ptr() const
		{
			return &_mtx;
		}

		template<typename req_lock>
		using auto_lock_t = typename safe_ptr<T, M, x_lock_t, s_lock_t>::template auto_lock_t<req_lock>;

		template<typename req_lock>
		using auto_lock_obj_t = typename safe_ptr<T, M, x_lock_t, s_lock_t>::template auto_lock_obj_t<req_lock>;

		using auto_nolock_t = typename safe_ptr<T, M, x_lock_t, s_lock_t>::auto_nolock_t;

		template<typename some_type>
		friend struct xlocked_safe_ptr;

		template<typename some_type>
		friend struct slocked_safe_ptr;

	public:
		template<typename... Args>
		safe_obj(Args... args)
		:
			_obj(args...)
		{

		}

		safe_obj(safe_obj const& safe_obj)
		{
			std::lock_guard<M> lock(safe_obj._mtx);
			_obj = safe_obj._obj;
		}

		explicit operator T() const
		{
			s_lock_t lock(_mtx);
			T obj_tmp = _obj;
			return obj_tmp;
		}

		auto_lock_t<x_lock_t> operator->()
		{
			return auto_lock_t<x_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		auto_lock_obj_t<x_lock_t> operator*()
		{
			return auto_lock_obj_t<x_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		const auto_lock_t<s_lock_t> operator->() const
		{
			return auto_lock_t<s_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		const auto_lock_obj_t<s_lock_t> operator*() const
		{
			return auto_lock_obj_t<s_lock_t>(get_obj_ptr(), *get_mtx_ptr());
		}

		using mtx_t = M;
		using obj_t = T;
		using xlock_t = x_lock_t;
		using slock_t = s_lock_t;
	};

	template<typename T, typename M = std::recursive_mutex, typename x_lock_t = std::unique_lock<M>, typename s_lock_t = std::unique_lock<M>>
	class safe_hide_ptr : protected safe_ptr<T, M, x_lock_t, s_lock_t>
	{
	public:
		template<typename... Args>
		safe_hide_ptr(Args... args)
		:
			safe_ptr<T, M, x_lock_t, s_lock_t>(args...)
		{

		}

		friend struct link_safe_ptrs;
		template<typename, typename, size_t, size_t>
		friend class lock_timed_transaction;
		template<typename some_type>
		friend struct xlocked_safe_ptr;
		template<typename some_type>
		friend struct slocked_safe_ptr;

		template<typename req_lock>
		using auto_lock_t = typename safe_ptr<T, M, x_lock_t, s_lock_t>::template auto_lock_t<req_lock>;
		template<typename req_lock>
		using auto_lock_obj_t = typename safe_ptr<T, M, x_lock_t, s_lock_t>::template auto_lock_obj_t<req_lock>;
		using auto_nolock_t = typename safe_ptr<T, M, x_lock_t, s_lock_t>::auto_nolock_t;
		using mtx_t = M;
		using obj_t = T;
		using xlock_t = x_lock_t;
		using slock_t = s_lock_t;
	};

	template<typename T, typename M = std::recursive_mutex, typename x_lock_t = std::unique_lock<M>, typename s_lock_t = std::unique_lock<M>>
	class safe_hide_obj : protected safe_obj<T, M, x_lock_t, s_lock_t>
	{
	public:
		template<typename... Args>
		safe_hide_obj(Args... args)
		:
			safe_obj<T, M, x_lock_t, s_lock_t>(args...)
		{

		}
		explicit operator T() const
		{
			return static_cast<safe_obj<T, M, x_lock_t, s_lock_t>>(*this);
		}

		friend struct link_safe_ptrs;
		template<typename, typename, size_t, size_t>

		friend class lock_timed_transaction;
		template<typename some_type>

		friend struct xlocked_safe_ptr;
		template<typename some_type>

		friend struct slocked_safe_ptr;
		template<typename req_lock>

		using auto_lock_t = typename safe_obj<T, M, x_lock_t, s_lock_t>::template auto_lock_t<req_lock>;

		template<typename req_lock>
		using auto_lock_obj_t = typename safe_obj<T, M, x_lock_t, s_lock_t>::template auto_lock_obj_t<req_lock>;

		using auto_nolock_t = typename safe_obj<T, M, x_lock_t, s_lock_t>::auto_nolock_t;
		using mtx_t = M;
		using obj_t = T;
		using xlock_t = x_lock_t;
		using slock_t = s_lock_t;
	};

	struct link_safe_ptrs
	{
		template<typename T1, typename... Args>
		link_safe_ptrs(T1 &first_ptr, Args&... args)
		{
			std::lock_guard<T1> lock(first_ptr);
			using M = typename T1::mtx_t;
			std::shared_ptr<M> old_mtxs[] = { args.mtx_ptr ... };
			std::shared_ptr<std::lock_guard<M>> locks[] = { std::make_shared<std::lock_guard<M>>(*args.mtx_ptr) ... };
			std::shared_ptr<M> mtxs[] = { (args.mtx_ptr = first_ptr.mtx_ptr) ... };
		}
	};

	enum lock_count_t
	{
		lock_once,
		lock_infinity
	};

	template<size_t lock_count, typename duration = std::chrono::nanoseconds, size_t deadlock_timeout = 100000, size_t spin_iterations = 100>
	class lock_timed_any
	{
	private:
		std::vector<std::shared_ptr<void>> _locks_ptr_vec;
		bool _success;

		template<typename mtx_t>
		std::unique_lock<mtx_t> try_lock_one(mtx_t &mtx) const
		{
			std::unique_lock<mtx_t> lock(mtx, std::defer_lock_t());
			for (size_t i = 0; i < spin_iterations; ++i) if (lock.try_lock()) return lock;
			const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
			//while (!lock.try_lock_for(duration(deadlock_timeout)))    // only for timed mutexes
			while (!lock.try_lock()) {
				auto const time_remained = duration(deadlock_timeout) - std::chrono::duration_cast<duration>(std::chrono::steady_clock::now() - start_time);
				if (time_remained <= duration(0))
					break;
				else
					std::this_thread::sleep_for(time_remained);
			}
			return lock;
		}

		template<typename mtx_t>
		std::shared_ptr<std::unique_lock<mtx_t>> try_lock_ptr_one(mtx_t &mtx) const
		{
			return std::make_shared<std::unique_lock<mtx_t>>(try_lock_one(mtx));
		}

	public:
		template<typename... Args>
		lock_timed_any(Args& ...args)
		{
			do {
				_success = true;
				for (auto &lock_ptr : { try_lock_ptr_one(*args._mtx_ptr.get()) ... }) {
					_locks_ptr_vec.emplace_back(lock_ptr);
					if (!lock_ptr->owns_lock()) {
						_success = false;
						_locks_ptr_vec.clear();
						std::this_thread::sleep_for(duration(deadlock_timeout));
						break;
					}
				}
			} while (!_success && lock_count == lock_count_t::lock_infinity);
		}

		explicit operator bool() const noexcept
		{
			return _success;
		}

		lock_timed_any(lock_timed_any&& other) noexcept
		:
			_locks_ptr_vec(other._locks_ptr_vec)
		{

		}

		lock_timed_any(const lock_timed_any&) = delete;
		lock_timed_any& operator=(const lock_timed_any&) = delete;
	};

	using lock_timed_any_once = lock_timed_any<lock_count_t::lock_once>;
	using lock_timed_any_infinity = lock_timed_any<lock_count_t::lock_infinity>;

	template<typename T>
	struct xlocked_safe_ptr
	{
		T& ref_safe;
		typename T::xlock_t xlock;

		xlocked_safe_ptr(T const& p)
		:
			ref_safe(*const_cast<T*>(&p)), xlock(*(ref_safe.get_mtx_ptr()))
		{

		}// ++xp;}

		typename T::obj_t* operator->()
		{
			return ref_safe.get_obj_ptr();
		}

		typename T::auto_nolock_t operator*()
		{
			return typename T::auto_nolock_t(ref_safe.get_obj_ptr(), *ref_safe.get_mtx_ptr());
		}

		operator typename T::obj_t()
		{
			return ref_safe.obj;
		} // only for safe_obj
	};

	template<typename T>
	xlocked_safe_ptr<T> xlock_safe_ptr(T const& arg)
	{
		return xlocked_safe_ptr<T>(arg);
	}

	template<typename T>
	struct slocked_safe_ptr
	{
		T &ref_safe;
		typename T::slock_t slock;
		slocked_safe_ptr(T const& p)
		:
			ref_safe(*const_cast<T*>(&p)), slock(*(ref_safe.get_mtx_ptr()))
		{

		}//++sp;}

		typename T::obj_t const* operator->() const
		{
			return ref_safe.get_obj_ptr();
		}

		const typename T::auto_nolock_t operator*() const
		{
			return typename T::auto_nolock_t(ref_safe.get_obj_ptr(), *ref_safe.get_mtx_ptr());
		}

		operator typename T::obj_t() const
		{
			return ref_safe.obj;
		} // only for safe_obj
	};

	template<typename T>
	slocked_safe_ptr<T> slock_safe_ptr(T const& arg)
	{
		return slocked_safe_ptr<T>(arg);
	}

	class spinlock_t
	{
		std::atomic_flag lock_flag;

	public:
		spinlock_t()
		{
			lock_flag.clear();
		}

		bool try_lock()
		{
			return !lock_flag.test_and_set(std::memory_order_acquire);
		}

		void lock()
		{
			for (volatile size_t i = 0; !try_lock(); ++i) {
				if (i % 100000 == 0) std::this_thread::yield();
			}
		}
		void unlock()
		{
			lock_flag.clear(std::memory_order_release);
		}
	};
	// ---------------------------------------------------------------

	class recursive_spinlock_t
	{
		std::atomic_flag _lock_flag;
		int64_t _recursive_counter;
#if (_WIN32 && _MSC_VER < 1900)
		typedef int64_t thread_id_t;
		std::atomic<thread_id_t> owner_thread_id;
		int64_t get_fast_this_thread_id() {
			static __declspec(thread) int64_t fast_this_thread_id = 0;  // MSVS 2013 thread_local partially supported - only POD
			if (fast_this_thread_id == 0) {
				std::stringstream ss;
				ss << std::this_thread::get_id();   // https://connect.microsoft.com/VisualStudio/feedback/details/1558211
				fast_this_thread_id = std::stoll(ss.str());
			}
			return fast_this_thread_id;
		}
#else
		typedef std::thread::id thread_id_t;
		std::atomic<std::thread::id> _owner_thread_id;

		std::thread::id get_fast_this_thread_id()
		{
			return std::this_thread::get_id();
		}
#endif

	public:
		recursive_spinlock_t()
		:
			_recursive_counter(0),
			_owner_thread_id(thread_id_t())
		{
			_lock_flag.clear();
		}

		bool try_lock()
		{
			if (!_lock_flag.test_and_set(std::memory_order_acquire)) {
				_owner_thread_id.store(get_fast_this_thread_id(), std::memory_order_release);
			}
			else {
				if (_owner_thread_id.load(std::memory_order_acquire) != get_fast_this_thread_id())
					return false;
			}
			++_recursive_counter;
			return true;
		}

		void lock()
		{
			for (volatile size_t i = 0; !try_lock(); ++i)
				if (i % 100000 == 0) std::this_thread::yield();
		}

		void unlock()
		{
			assert(_owner_thread_id.load(std::memory_order_acquire) == get_fast_this_thread_id());
			assert(_recursive_counter > 0);

			if (--_recursive_counter == 0) {
				_owner_thread_id.store(thread_id_t(), std::memory_order_release);
				_lock_flag.clear(std::memory_order_release);
			}
		}
	};
	// ---------------------------------------------------------------

	// contention free shared mutex (same-lock-type is recursive for X->X, X->S or S->S locks), but (S->X - is UB)
	template<unsigned contention_free_count = 36, bool shared_flag = false>
	class contention_free_shared_mutex
	{
		std::atomic<bool> _want_x_lock;
		//struct cont_free_flag_t { alignas(std::hardware_destructive_interference_size) std::atomic<int> value; cont_free_flag_t() { value = 0; } }; // C++17
		struct cont_free_flag_t
		{
			char tmp[60];
			std::atomic<int> value;

			cont_free_flag_t()
			{
				value = 0;
			}
		}; // tmp[] to avoid false sharing
		typedef std::array<cont_free_flag_t, contention_free_count> array_slock_t;

		const std::shared_ptr<array_slock_t> _shared_locks_array_ptr;  // 0 - unregistred, 1 registred & free, 2... - busy
		char _avoid_falsesharing_1[64];

		array_slock_t& _shared_locks_array;
		char _avoid_falsesharing_2[64];

		int _recursive_xlock_count;


		enum index_op_t
		{
			unregister_thread_op,
			get_index_op,
			register_thread_op
		};

#if (_WIN32 && _MSC_VER < 1900) // only for MSVS 2013
		typedef int64_t thread_id_t;
		std::atomic<thread_id_t> owner_thread_id;
		std::array<int64_t, contention_free_count> register_thread_array;
		int64_t get_fast_this_thread_id() {
			static __declspec(thread) int64_t fast_this_thread_id = 0;  // MSVS 2013 thread_local partially supported - only POD
			if (fast_this_thread_id == 0) {
				std::stringstream ss;
				ss << std::this_thread::get_id();   // https://connect.microsoft.com/VisualStudio/feedback/details/1558211
				fast_this_thread_id = std::stoll(ss.str());
			}
			return fast_this_thread_id;
		}

		int get_or_set_index(index_op_t index_op = get_index_op, int set_index = -1) {
			if (index_op == get_index_op) {  // get index
				auto const thread_id = get_fast_this_thread_id();

				for (size_t i = 0; i < register_thread_array.size(); ++i) {
					if (register_thread_array[i] == thread_id) {
						set_index = i;   // thread already registred
						break;
					}
				}
			}
			else if (index_op == register_thread_op) {  // register thread
				register_thread_array[set_index] = get_fast_this_thread_id();
			}
			return set_index;
		}

#else
		typedef std::thread::id thread_id_t;
		std::atomic<std::thread::id> _owner_thread_id;
		std::thread::id get_fast_this_thread_id()
		{
			return std::this_thread::get_id();
		}

		struct unregister_t
		{
			int thread_index;
			std::shared_ptr<array_slock_t> array_slock_ptr;
			unregister_t(int index, std::shared_ptr<array_slock_t> const& ptr) : thread_index(index), array_slock_ptr(ptr) {}
			unregister_t(unregister_t &&src) : thread_index(src.thread_index), array_slock_ptr(std::move(src.array_slock_ptr)) {}
			~unregister_t() { if (array_slock_ptr.use_count() > 0) (*array_slock_ptr)[thread_index].value--; }
		};

		int get_or_set_index(index_op_t index_op = get_index_op, int set_index = -1)
		{
			thread_local static std::unordered_map<void *, unregister_t> thread_local_index_hashmap;
			// get thread index - in any cases
			auto it = thread_local_index_hashmap.find(this);
			if (it != thread_local_index_hashmap.cend()) {
				set_index = it->second.thread_index;
			}
			if (index_op == unregister_thread_op) {  // unregister thread
				if (_shared_locks_array[set_index].value == 1) { // if isn't shared_lock now
					thread_local_index_hashmap.erase(this);
				} else {
					return -1;
				}
			} else if (index_op == register_thread_op) {  // register thread
				thread_local_index_hashmap.emplace(this, unregister_t(set_index, _shared_locks_array_ptr));

				// remove info about deleted contfree-mutexes
				for (auto it = thread_local_index_hashmap.begin(), ite = thread_local_index_hashmap.end(); it != ite;) {
					if (it->second.array_slock_ptr->at(it->second.thread_index).value < 0) { /// < if contfree-mtx was deleted
						it = thread_local_index_hashmap.erase(it);
					} else {
						++it;
					}
				}
			}
			return set_index;
		}

#endif

		public:
			contention_free_shared_mutex()
			:
				_shared_locks_array_ptr(std::make_shared<array_slock_t>()),
				_shared_locks_array(*_shared_locks_array_ptr),
				_want_x_lock(false),
				_recursive_xlock_count(0),
				_owner_thread_id(thread_id_t())
			{

			}

			~contention_free_shared_mutex()
			{
				for (auto &i : _shared_locks_array) i.value = -1;
			}

			bool unregister_thread()
			{
				return get_or_set_index(unregister_thread_op) >= 0;
			}

			int register_thread()
			{
				int cur_index = get_or_set_index();

				if (cur_index == -1) {
					if (_shared_locks_array_ptr.use_count() <= (int)_shared_locks_array.size())  // try once to register thread
					{
						for (size_t i = 0; i < _shared_locks_array.size(); ++i) {
							int unregistred_value = 0;
							if (_shared_locks_array[i].value == 0)
								if (_shared_locks_array[i].value.compare_exchange_strong(unregistred_value, 1)) {
									cur_index = i;
									get_or_set_index(register_thread_op, cur_index);   // thread registred success
									break;
								}
						}
						//std::cout << "\n thread_id = " << std::this_thread::get_id() << ", register_thread_index = " << cur_index <<
						//    ", shared_locks_array[cur_index].value = " << shared_locks_array[cur_index].value << std::endl;
					}
				}
				return cur_index;
			}

			void lock_shared()
			{
				int const register_index = register_thread();

				if (register_index >= 0) {
					int recursion_depth = _shared_locks_array[register_index].value.load(std::memory_order_acquire);
					assert(recursion_depth >= 1);
					if (recursion_depth > 1) {  /// < if recursive -> release
						_shared_locks_array[register_index].value.store(recursion_depth + 1, std::memory_order_release);
					} else { /// < if first -> sequential
						_shared_locks_array[register_index].value.store(recursion_depth + 1, std::memory_order_seq_cst);
						while (_want_x_lock.load(std::memory_order_seq_cst)) {
							_shared_locks_array[register_index].value.store(recursion_depth, std::memory_order_seq_cst);
							for (volatile size_t i = 0; _want_x_lock.load(std::memory_order_seq_cst); ++i)
								if (i % 100000 == 0) std::this_thread::yield();
							_shared_locks_array[register_index].value.store(recursion_depth + 1, std::memory_order_seq_cst);
						}
					}
					// (shared_locks_array[register_index] == 2 && want_x_lock == false) ||     // first shared lock
					// (shared_locks_array[register_index] > 2)                                 // recursive shared lock
				} else {
					if (_owner_thread_id.load(std::memory_order_acquire) != get_fast_this_thread_id()) {
						size_t i = 0;
						for (bool flag = false; !_want_x_lock.compare_exchange_weak(flag, true, std::memory_order_seq_cst); flag = false) {
							if (++i % 100000 == 0) {
								std::this_thread::yield();
							}
						}
						_owner_thread_id.store(get_fast_this_thread_id(), std::memory_order_release);
					}
					++_recursive_xlock_count;
				}
			}

			void unlock_shared()
			{
				int const register_index = get_or_set_index();
				if (register_index >= 0) {
					int const recursion_depth = _shared_locks_array[register_index].value.load(std::memory_order_acquire);
					assert(recursion_depth > 1);

					_shared_locks_array[register_index].value.store(recursion_depth - 1, std::memory_order_release);
				} else {
					if (--_recursive_xlock_count == 0) {
						_owner_thread_id.store(decltype(_owner_thread_id)(), std::memory_order_release);
						_want_x_lock.store(false, std::memory_order_release);
					}
				}
			}

			void lock()
			{
				// forbidden upgrade S-lock to X-lock - this is an excellent opportunity to get deadlock
				int const register_index = get_or_set_index();
				if (register_index >= 0) {
					assert(_shared_locks_array[register_index].value.load(std::memory_order_acquire) == 1);
				}
				if (_owner_thread_id.load(std::memory_order_acquire) != get_fast_this_thread_id()) {
					size_t i = 0;
					for (bool flag = false; !_want_x_lock.compare_exchange_weak(flag, true, std::memory_order_seq_cst); flag = false) {
						if (++i % 1000000 == 0) std::this_thread::yield();
					}

					_owner_thread_id.store(get_fast_this_thread_id(), std::memory_order_release);

					for (auto &i : _shared_locks_array) {
						while (i.value.load(std::memory_order_seq_cst) > 1);
					}
				}
				++_recursive_xlock_count;
			}

			void unlock()
			{
				assert(_recursive_xlock_count > 0);

				if (--_recursive_xlock_count == 0) {
					_owner_thread_id.store(decltype(_owner_thread_id)(), std::memory_order_release);
					_want_x_lock.store(false, std::memory_order_release);
				}
			}
	};

	template<typename M>
	struct shared_lock_guard
	{
		M& _ref_mtx;

		shared_lock_guard(M &mtx)
		:
			_ref_mtx(mtx)
		{
			_ref_mtx.lock_shared();
		}

		~shared_lock_guard()
		{
			_ref_mtx.unlock_shared();
		}
	};

	using default_contention_free_shared_mutex = contention_free_shared_mutex<>;

	template<typename T>
	using contfree_safe_ptr = safe_ptr<T, contention_free_shared_mutex<>, std::unique_lock<contention_free_shared_mutex<>>, shared_lock_guard<contention_free_shared_mutex<>>>;

	// safe partitioned map
	template<typename key_t, typename val_t, template<class> class safe_ptr_t = default_safe_ptr, typename container_t = std::map<key_t, val_t>, typename part_t = std::map<key_t, safe_ptr_t<container_t>>>
	class safe_map_partitioned_t
	{
		using safe_container_t = safe_ptr_t<container_t>;
		using part_iterator = typename part_t::iterator ;
		using const_part_iterator = typename part_t::const_iterator;
		std::shared_ptr<part_t> _partition;

	public:
		using result_vector_t = std::vector<std::pair<key_t, val_t>>;

		safe_map_partitioned_t()
		:
			_partition(std::make_shared<part_t>())
		{
			_partition->emplace(key_t(), container_t());
		}

		safe_map_partitioned_t(const key_t start, const key_t end, const key_t step)
		:
			_partition(std::make_shared<part_t>())
		{
			for (key_t i = start; i <= end; i += step)
			{
				_partition->emplace(i, container_t());
			}
		}

		safe_map_partitioned_t(std::initializer_list<key_t> const& il)
		:
			_partition(std::make_shared<part_t>())
		{
			for (auto &i : il) {
				_partition->emplace(i, container_t());
			}
		}

		part_iterator part_it(key_t const& k)
		{
			auto it = _partition->lower_bound(k);
			if (it == _partition->cend()) --it; {
				return it;
			}
		}

		const_part_iterator part_it(key_t const& k) const
		{
			auto it = _partition->lower_bound(k);
			if (it == _partition->cend()) --it; {
				return it;
			}
		}

		safe_container_t& part(key_t const& k)
		{
			return part_it(k)->second;
		}

		const safe_container_t& part(key_t const& k) const
		{
			return part_it(k)->second;
		}

		slocked_safe_ptr<safe_container_t> read_only_part(key_t const& k) const
		{
			return slock_safe_ptr(part(k));
		}

		void get_range_equal(const key_t& key, result_vector_t &result_vec) const
		{
			result_vec.clear();
			auto slock_container = slock_safe_ptr(part(key));
			for (auto it = slock_container->lower_bound(key); it != slock_container->upper_bound(key); ++it) {
				result_vec.emplace_back(*it);
			}
		}

		void get_range_lower_upper(const key_t& low, const key_t& up, result_vector_t &result_vec) const
		{
			result_vec.clear();
			auto const& const_part = *_partition;
			auto end_it = (const_part.upper_bound(up) == const_part.cend()) ? const_part.cend() : std::next(const_part.upper_bound(up), 1);
			auto it = const_part.lower_bound(low);
			if (it == const_part.cend()) {
				--it;
			}
			for (; it != end_it; ++it) {
				result_vec.insert(result_vec.end(), it->second->lower_bound(low), it->second->upper_bound(up));
			}
		}

		void erase_lower_upper(const key_t& low, const key_t& up)
		{
			auto end_it = (_partition->upper_bound(up) == _partition->end()) ? _partition->end() : std::next(_partition->upper_bound(up), 1);

			for (auto it = part_it(low); it != end_it; ++it) {
				it->second->erase(it->second->lower_bound(low), it->second->upper_bound(up));
			}
		}

		template<typename T, typename... Args> void emplace(T const& key, Args const&&...args)
		{
			part(key)->emplace(key, args...);
		}

		size_t size() const
		{
			size_t size = 0;
			for (auto it = _partition->begin(); it != _partition->end(); ++it) {
				size += it->second->size();
			}
			return size;
		}

		size_t erase(key_t const& key) noexcept
		{
			return part(key)->erase(key);
		}

		void clear()
		{
			for (auto it = _partition->begin(); it != _partition->end(); ++it) {
				it->second->clear();
			}
		}
	};

	template<typename T, typename ...TArgs>
	safe_ptr<T> make_safe(TArgs&& ...args)
	{
		return safe_ptr<T>(std::forward<TArgs>(args)...);
	}
}

namespace std {
	template<typename T>
	class hash<chaos::safe_ptr<T>>
	{
	public:
		uintptr_t operator()(const chaos::safe_ptr<T>& ptr) const
		{
			return !ptr? 0 : static_cast<std::uintptr_t>(std::hash<typename std::remove_const<T>::type*>()(ptr.get_obj_ptr()));
		}
	};
}

#endif
