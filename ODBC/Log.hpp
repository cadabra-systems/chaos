/**
 @file Log.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_ODBC_Log_hpp
#define Chaos_ODBC_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace odbc {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("ODBC")
		{
			
		}
		virtual ~log() override = default;
	/** @} */
	};
} }

#endif
