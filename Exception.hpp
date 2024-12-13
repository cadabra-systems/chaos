//
//  Exception.hpp
//
//  Created by Daniil A Megrabyan on 25.08.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_Exception_hpp
#define Chaos_Exception_hpp

#include <exception>
#include <ostream>
#include <sstream>

#include "ExceptionProvoker.hpp"

namespace chaos {
	class exception : public std::exception
	{
	/** @name Constructors */
	/** @{ */
	public:
		exception() = default;
		virtual ~exception() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::string _message;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		virtual const char* what() const noexcept final
		{
			return _message.c_str();
		}
		
		virtual const std::string& message() const noexcept final
		{
			return _message;
		}
		
		virtual const std::string& text() const noexcept final
		{
			return _message;
		}
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		template<typename T>
		exception& operator<<(const T &t)
		{
			std::stringstream ss;
			ss << t;
			_message += ss.str();
			
			return *this;
		}
		
		exception& operator<<(std::ostream& (*)(std::ostream&))
		{
			_message += "\n";
			
			return *this;
		}
	/** @} */
	};
}
#endif
