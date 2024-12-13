/**
 @file Log.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Redis_Log_hpp
#define Chaos_Redis_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace redis {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("Redis")
		{
			
		}
		virtual ~log() = default;
	/** @} */
	};
} }

#endif
