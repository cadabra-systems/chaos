/**
 @file DelCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_DelCommand_hpp
#define Chaos_Redis_Command_DelCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class del_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		del_command(std::string key);
		del_command(del_command const&) = delete;
		del_command& operator=(del_command const&) = delete;
		virtual ~del_command() override;
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
		bool is_completed() const;
	/** @} */
	};
} }

#endif

