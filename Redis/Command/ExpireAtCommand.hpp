/**
 @file ExpireAtCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_ExpireAtCommand_hpp
#define Chaos_Redis_Command_ExpireAtCommand_hpp

#include "../Command.hpp"

#include <chrono>

namespace chaos { namespace redis {
	class expireat_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		expireat_command(std::string key, std::chrono::system_clock::time_point timestamp);
		expireat_command(expireat_command const&) = delete;
		expireat_command& operator=(expireat_command const&) = delete;
		virtual ~expireat_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::chrono::system_clock::time_point _timestamp;
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
