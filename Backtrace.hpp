/**
 @file Backtrace.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Backtrace_hpp
#define Chaos_Backtrace_hpp

#define UNW_LOCAL_ONLY

#include <list>
#include <string>
#include <utility>
#include <cstdint>

namespace chaos {
	class backtrace
	{
	/** @name Statics */
	/** @{ */
	public:
		static std::list<std::pair<std::string, std::uintptr_t>> stack();
		static std::list<std::string> list();
	/** @} */
	};
}

#endif
