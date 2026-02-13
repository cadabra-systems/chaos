/**
 * @file ResetStringCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_ResetStringCommand_hpp
#define Chaos_Redis_Command_ResetStringCommand_hpp

#include "../Command.hpp"

#include <string>
#include <chrono>

namespace chaos { namespace redis {
	class reset_string_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		reset_string_command
		(
			const std::string& key,
			const std::string& value,
			const std::chrono::milliseconds& lifetime = std::chrono::milliseconds{0}
		);
		virtual ~reset_string_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::string _value;
		const std::chrono::milliseconds _lifetime;
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
