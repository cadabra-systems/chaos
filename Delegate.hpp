/**
 @file Delegate.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Delegate_hpp
#define Chaos_Delegate_hpp

#include <functional>

#include <list>

namespace chaos {
	template<typename F>
	class delegate : public std::function<F>
	{
	/** @name Aliases */
	/** @{ */
	public:
//		using ResultType = std::list<typename std::function<F>::ResultType>;
//		using AllocatorType = typename std::function<F>::AllocatorType;
	/** @} */

	/** @name Classes */
	/** @{ */
	private:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		explicit delegate()
		{
		
		}

		delegate(delegate&& origin)
		{
			_list.swap(origin._list);
		}

		/**
		 * @brief The implicitly generated copy constructor, copy assignment, and destructor are fine.
		 * @param f
		 * @return
		 */
		template<typename F2>
		delegate(std::function<F2> f)
		{
			_list.push_back(f);
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::list<std::function<F>> _list;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		/**
		 * @brief If you have an implementation of function that supports. Equality comparison, uncomment the calls to remove.
		 * @param f
		 * @return
		 */
		void add(std::function<F> f)
		{
//			std::shared_ptr<void>(sp, static_cast<void*>(sp.get()));

			_list.push_back(f);
		}

		void swap(delegate& other)
		{
			_list.swap(other._list);
		}

		void clear()
		{
			_list.clear();
		}
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		void operator+=(const delegate<F>& rhs)
		{
			_list.insert(_list.end(), rhs._list.begin(), rhs._list.end());
		}

		void operator+=(std::function<F> f)
		{
			_list.push_back(f);
		}

		template<typename... Ts>
		void operator()(Ts&& ...vs) const
		{
			for (auto i = begin(_list); i != end(_list); ++i) {
				(*i)(std::forward<Ts>(vs)... );
			}
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool empty() const
		{
			return _list.empty();
		}

		operator bool() const
		{
			return _list.empty();
		}
	/** @} */
	};

	template<typename MF1, typename MF2>
	void swap(delegate<MF1>& mf1, delegate<MF2>& mf2)
	{
		mf1.swap(mf2);
	}

	template<typename MF1, typename MF2>
	bool operator==(const delegate<MF1>&, const delegate<MF2>&)
	{
		return false;
	}

	template<typename MF1, typename MF2>
	bool operator!=(const delegate<MF1>& mf1, const delegate<MF2>& mf2)
	{
		return !(mf1 == mf2);
	}
}
#endif
