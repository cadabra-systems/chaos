/**
 @file PushXCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_PushXCommand_hpp
#define Chaos_Redis_Command_PushXCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	template <typename T>
	class pushx_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		pushx_command(std::string key, T member, list_ending ending = list_ending::right)
		:
			_key(key),
			_member(member),
			_ending(ending)
		{
			
		}
		
		pushx_command(pushx_command const&) = delete;
		pushx_command& operator=(pushx_command const&) = delete;
		virtual ~pushx_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const T _member;
		const list_ending _ending;
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
			return command_line(static_cast<char>(_ending) + std::string("PUSHX"), _key, _member);
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
		std::size_t get_length() const
		{
			if (get_reply() != reply::type::integer) {
				return 0;
			}
			
			return get_reply().get_integer();
		}
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
