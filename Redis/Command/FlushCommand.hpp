/**
 * @file FlushCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_FlushCommand_hpp
#define Chaos_Redis_Command_FlushCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class flush_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		flush_command(bool async_mode = false);
		virtual ~flush_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		bool _async_mode;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
