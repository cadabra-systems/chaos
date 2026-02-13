/**
 * @file UpdateStringCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_UpdateStringCommand_hpp
#define Chaos_Redis_Command_UpdateStringCommand_hpp

#include "../Command.hpp"

#include <string>
#include <chrono>

namespace chaos { namespace redis {
	class update_string_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		update_string_command
		(
			const std::string& key,
			const std::string& expected_value,
			const std::string& desired_value,
			const std::chrono::milliseconds& lifetime = std::chrono::milliseconds{0}
		);
		virtual ~update_string_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::string _expected_value;
		const std::string _desired_value;
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
