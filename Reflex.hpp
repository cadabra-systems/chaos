/**
 @file Reflex.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Reflex_hpp
#define Chaos_Reflex_hpp

#include "Flex.hpp"
#include "CRP.hpp"
#include "Demangler.hpp"
#include "Delegate.hpp"
#include "IsDetected.hpp"

#include <tuple>
#include <string>
#include <type_traits>
#include <sstream>

namespace chaos {
	template <typename T>
	class reflex_action
	{
	template <typename U>
	static void detect(...);

	template <typename U>
	static typename U::_ detect(typename U::_*);

	public:
		using reaction_type = decltype(detect<T>(nullptr));
	};

	template <typename A>
	class reflex_reaction;

	template<typename A>
	class basic_reflex
	{
	/** @name Aliases */
	/** @{ */
	protected:
		using reaction_type = typename reflex_action<A>::reaction_type;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		virtual ~basic_reflex() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		chaos::delegate<reaction_type(const A&)> _mirror;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:
		void mirror(const A& action) const
		{
			_mirror(action);
		}
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void add(std::function<reaction_type(const typename std::remove_const<A>::type&)> delegate)
		{
			_mirror.add(delegate);
		}
	/** @} */
	};

	template<typename R>
	class const_reflex : public basic_reflex<R>
	{
	/** @name Aliases */
	/** @{ */
	protected:
	/** @}*/

	/** @name Constructors */
	/** @{ */
	public:
		virtual ~const_reflex() = default;
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
		virtual typename basic_reflex<R>::reaction_type react(const typename std::remove_const<R>::type& action) const
		{
			return typename basic_reflex<R>::reaction_type();
		}
	/** @} */
	};

	template<typename R>
	class mutable_reflex : public basic_reflex<R>
	{
	/** @name Aliases */
	/** @{ */
	protected:
	/** @}*/

	/** @name Constructors */
	/** @{ */
	public:
		virtual ~mutable_reflex() = default;
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
		virtual typename basic_reflex<R>::reaction_type react(const typename std::remove_const<R>::type& action)
		{
			return typename basic_reflex<R>::reaction_type();
		}
	/** @} */
	};

	template <typename R>
	using deduced_reflex = typename std::conditional<std::is_const<R>::value, const_reflex<R>, mutable_reflex<R>>::type;

	template<typename...>
	class reflex;

	template<typename R>
	class reflex<R> : public deduced_reflex<R>
	{
	/** @name Aliases */
	/** @{ */
	public:
		using deduced_reflex<R>::react;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		virtual ~reflex() = default;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		bool re(const crp_action& action, crp_reaction& reaction)
		{
			if (action.reflector().method().compare(chaos::demangler::name(typeid(R))) != 0) {
				return false;
			}

			const R payload(action.payload());
			reaction.set(react(payload));
			/// @todo mirror(action);
			
			return true;
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
		
	/** @name Hooks */
	/** @{ */
	public:
	/** @} */
	};
	
	template<typename R, typename... Rs>
	class reflex<R, Rs...> : public reflex<R>, reflex<Rs...>
	{
	/** @name Aliases */
	/** @{ */
	public:
		using reflex<R>::react;
		using reflex<Rs...>::react;
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		reflex()
		:
			reflex<Rs...>()/*,
			_tuple(R{}, Rs{}...)*/
		{
			
		}
		
		virtual ~reflex() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		//std::tuple<const R, const Rs...> _tuple;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	private:
		template<std::size_t I = 0>
		typename std::enable_if<I == sizeof...(Rs), bool>::type
		re(const crp_action&, crp_reaction&)
		{
			return false;
		}

		template<std::size_t I = 0>
		typename std::enable_if<I < sizeof...(Rs), bool>::type
		re(const crp_action& action, crp_reaction& reaction)
		{
			using action_type = typename std::tuple_element<I, std::tuple<Rs...>>::type;
			if (action.reflector().method().compare(chaos::demangler::name(typeid(action_type))) != 0) {
				return re<I + 1>(action, reaction);
			}

			const action_type payload(action.payload());
			reaction.set(react(payload));

			return true;
		}
		
	public:
		bool re(const chaos::crp_action& action, chaos::crp_reaction& reaction)
		{
			if (action.reflector().method().compare(chaos::demangler::name(typeid(R))) != 0) {
				return re<0>(action, reaction);
			}

			const R payload(action.payload());
			reaction.set(react(payload));

			return true;
		}
	/** @} */
	};
		
	template<>
	class reflex<>
	{
	/** @name Procedures */
	/** @{ */
	public:
		bool re(const crp_action&, crp_reaction&)
		{
			return false;
		}
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
		void react()
		{
			return ;
		}
	/** @} */
	};

	template<typename R1, typename R2>
	bool operator==(const reflex<R1>&, const reflex<R2>&)
	{
		return false;
	}

	template<typename R1, typename R2>
	bool operator!=(const reflex<R1>& action1, const reflex<R2>& action2)
	{
		return !(action1 == action2);
	}
}
#endif
