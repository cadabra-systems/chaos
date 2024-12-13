/**
 @file ExpireCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_ExpireCommand_hpp
#define Chaos_Redis_Command_ExpireCommand_hpp

#include "../Command.hpp"

#include <chrono>

namespace chaos { namespace redis {
	class expire_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		expire_command(std::string key, std::chrono::seconds seconds);
		expire_command(expire_command const&) = delete;
		expire_command& operator=(expire_command const&) = delete;
		virtual ~expire_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::chrono::seconds _seconds;
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
		bool is_completed() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
