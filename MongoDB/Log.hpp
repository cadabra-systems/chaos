/**
 @file Log.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
 */

#ifndef Chaos_MongoDB_Log_hpp
#define Chaos_MongoDB_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace mongodb {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("MongoDB")
		{
			
		}
		virtual ~log() = default;
	/** @} */
	};
} }

#endif
