/**
 @file HMGetCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_HMGetCommand_hpp
#define Chaos_Redis_Command_HMGetCommand_hpp

#include "../Command.hpp"

#include <list>

namespace chaos { namespace redis {
	class hmget_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		hmget_command(std::string key, std::list<const std::string>& list);
		hmget_command(std::string key, const std::string& member);
		hmget_command(hmget_command const&) = delete;
		hmget_command& operator=(hmget_command const&) = delete;
		virtual ~hmget_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::list<const std::string> _list;
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
		std::size_t get_count() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
