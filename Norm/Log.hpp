/**
 @file Log.hpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author danil@megrabyan.pro
 */

#ifndef Chaos_CDO_Log_hpp
#define Chaos_CDO_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace CDO {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("CDO")
		{
			
		}
		virtual ~log() = default;
	/** @} */
	};
} }

#endif
