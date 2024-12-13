/**
 @file Log.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
 */

#ifndef Chaos_CWS_Log_hpp
#define Chaos_CWS_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace cws {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("CWS")
		{
			
		}
		virtual ~log() = default;
	/** @} */
	};
} }

#endif

