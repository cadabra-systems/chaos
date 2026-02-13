/**
 * @file PingCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_PingCommand_hpp
#define Chaos_Redis_Command_PingCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class ping_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		ping_command();
		virtual ~ping_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
