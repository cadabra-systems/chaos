/**
 @file PushCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_PushCommand_hpp
#define Chaos_Redis_Command_PushCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class push_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class list_ending : char
		{
			left = 'L',
			right = 'R'
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		push_command(std::string key, std::initializer_list<std::string> list, push_command::list_ending ending = push_command::list_ending::right);
		push_command(std::string key, std::string member, push_command::list_ending ending = push_command::list_ending::right);
		push_command(push_command const&) = delete;
		push_command& operator=(push_command const&) = delete;
		virtual ~push_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::list<std::string> _list;
		const push_command::list_ending _ending;
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
		std::size_t get_length() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
