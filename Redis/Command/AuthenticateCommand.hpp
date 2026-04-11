/**
 * @file AuthenticateCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_AuthenticateCommand_hpp
#define Chaos_Redis_Command_AuthenticateCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class authenticate_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		authenticate_command(const std::string& password);
		authenticate_command(const std::string& username, const std::string& password);
		virtual ~authenticate_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _username;
		const std::string _password;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
