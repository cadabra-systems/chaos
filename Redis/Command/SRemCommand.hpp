/**
 @file SRemCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_SRemCommand_hpp
#define Chaos_Redis_Command_SRemCommand_hpp

#include "../Command.hpp"

#include <chrono>

namespace chaos { namespace redis {
	template<typename T>
	class srem_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		srem_command(std::string key, T member)
		:
			_key(key),
			_member(member)
		{
			
		}
		srem_command(srem_command const&) = delete;
		srem_command& operator=(srem_command const&) = delete;
		virtual ~srem_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const T _member;
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
			return command_line("SREM", _key, _member);
		}
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
		bool is_completed() const
		{
			if (get_reply() != reply::type::integer) {
				return false;
			}
			
			return (1 == get_reply().get_integer());
		}
	/** @} */
	};
} }

#endif
