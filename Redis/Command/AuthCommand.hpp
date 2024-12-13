/**
 @file AuthCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_AuthCommand_hpp
#define Chaos_Redis_Command_AuthCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class auth_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		auth_command(std::string password);
		auth_command(auth_command const&) = delete;
		auth_command& operator=(auth_command const&) = delete;
		virtual ~auth_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _password;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual command_line make_line() const override
		{
			return command_line("AUTH", _password);
		};
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:

	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool is_success() const;
	/** @} */
	};
} }

#endif
