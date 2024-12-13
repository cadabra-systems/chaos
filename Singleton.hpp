/**
 @file Singleton.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Singleton_hpp
#define Chaos_Singleton_hpp

#include <memory>
#include <utility>

namespace chaos {
	template<typename T>
	class singleton
	{
	/** @name Statics */
	/** @{ */
	public:
		template<typename ...TArgs>
		static T& instance(TArgs&& ...args)
		{
			static singleton<T> retval(std::forward<TArgs>(args)...);
			return *(retval._instance.get());
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		singleton()
		:
			_instance(new T())
		{

		}

		template<typename ...TArgs>
		singleton(TArgs&& ...args)
		:
			_instance(new T(std::forward<TArgs>(args)...))
		{

		}

		~singleton() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::unique_ptr<T> _instance;
	/** @} */
	};
}

#endif
