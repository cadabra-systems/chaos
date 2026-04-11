/**
 * @file SelectCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_SelectCommand_hpp
#define Chaos_Redis_Command_SelectCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class select_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		select_command(std::uint8_t index);
		virtual ~select_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _index;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
