/**
 @file Exchange.hpp
 @date 2026-04-22
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Exchange_hpp
#define Chaos_RabbitMQ_Exchange_hpp

#include "../BiMap.hpp"

namespace chaos { namespace rabbitmq {
	class exchange
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class type : char
		{
			fanout = 'F',
			direct = 'D',
			topic = 'T',
			meta = 'M'
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static const chaos::bimap<exchange::type, std::string> type_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		exchange() = default;

	public:
		~exchange() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
