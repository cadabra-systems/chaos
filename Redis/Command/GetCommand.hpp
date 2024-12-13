/**
 @file GetCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_GetCommand_hpp
#define Chaos_Redis_Command_GetCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class get_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		get_command(std::string key);
		get_command(get_command const&) = delete;
		get_command& operator=(get_command const&) = delete;
		virtual ~get_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual command_line make_line() const override;
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
	/** @} */
	};
} }

#endif
