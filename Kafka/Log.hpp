/**
 @file Log.hpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Kafka_Log_hpp
#define Chaos_Kafka_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace kafka {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("Kafka")
		{

		}
		virtual ~log() = default;
	/** @} */
	};
} }

#endif
