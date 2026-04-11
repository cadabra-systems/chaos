/**
 * @file SetClientNameCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_SetClientNameCommand_hpp
#define Chaos_Redis_Command_SetClientNameCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class set_client_name_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		set_client_name_command(const std::string& value);
		virtual ~set_client_name_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _value;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
