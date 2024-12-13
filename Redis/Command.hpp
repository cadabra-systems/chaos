/**
 @file Command.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_hpp
#define Chaos_Redis_Command_hpp

#include "Redis.hpp"
#include "Reply.hpp"
#include "CommandLine.hpp"

#include <memory>
#include <list>
#include <utility>
#include <string>

namespace chaos { namespace redis {
	class error;
	class connection;
	
	class command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		command();
		command(command const&) = delete;
		command& operator=(command const&) = delete;
		virtual ~command();
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::unique_ptr<reply> _reply;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual command_line make_line() const = 0;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		const reply* operator->() const;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
		void reset_reply(std::unique_ptr<reply> reply);
	/** @} */
		
	/** @name Getters */
	/** @{ */
	protected:
		const reply& get_reply() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool has_reply() const;
	/** @} */
	};
} }

#endif
