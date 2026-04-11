/**
 * @file LoadStringCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_LoadStringCommand_hpp
#define Chaos_Redis_Command_LoadStringCommand_hpp

#include "../Command.hpp"

#include <string>

namespace chaos { namespace redis {
	class load_string_command : public function<std::string>
	{
	/** @name Constructors */
	/** @{ */
	public:
		load_string_command(const std::string& key);
		virtual ~load_string_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
