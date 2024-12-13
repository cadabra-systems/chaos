//
//  ExceptionProvoker.hpp
//
//  Created by Daniil A Megrabyan on 25.08.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_ExceptionProvoker_hpp
#define Chaos_ExceptionProvoker_hpp

#include <ostream>

namespace chaos {
	template<typename E>
	class exception_provoker
	{
	/** @name Constructors */
	/** @{ */
	public:
		exception_provoker()
		{
			
		}
		
		template<typename ...TArgs>
		exception_provoker(TArgs&& ...args)
		:
			_payload(std::forward<TArgs>(args)... )
		{
			
		}
		
		exception_provoker(const exception_provoker<E>& orig) = delete;
		exception_provoker(exception_provoker<E>&& orig) = delete;
		
		~exception_provoker() noexcept
		{
			
		}
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		E _payload;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Templates */
	/** @{ */
	public:
		template<typename T>
		exception_provoker<E>& operator<<(const T &t)
		{
			_payload << t;
			
			return *this;
		}
		
		E& operator<<(std::ostream& (*)(std::ostream&))
		{
			return _payload;
		}
	/** @} */
	};
}

#endif
