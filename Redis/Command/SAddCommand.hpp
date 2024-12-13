/**
 @file SAddCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_SAddCommand_hpp
#define Chaos_Redis_Command_SAddCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class sadd_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		sadd_command(std::string key, std::initializer_list<std::string> list);
		sadd_command(std::string key, std::string member);
		sadd_command(sadd_command const&) = delete;
		sadd_command& operator=(sadd_command const&) = delete;
		virtual ~sadd_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::list<std::string> _list;
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
		std::size_t get_affected_members() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
