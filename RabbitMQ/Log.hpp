/**
 @file Log.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
 */

#ifndef Chaos_RabbitMQ_Log_hpp
#define Chaos_RabbitMQ_Log_hpp

#include "../Log.hpp"

namespace chaos { namespace rabbitmq {
	class log : public chaos::log
	{
	/** @name Constructors */
	/** @{ */
	public:
		log()
		:
			chaos::log("RabbitMQ")
		{
			
		}
		virtual ~log() = default;
	/** @} */
	};
} }

#endif
