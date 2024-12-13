/**
 @file ExistsCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_ExistsCommand_hpp
#define Chaos_Redis_Command_ExistsCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class exists_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		exists_command(std::string key);
		exists_command(exists_command const&) = delete;
		exists_command& operator=(exists_command const&) = delete;
		virtual ~exists_command() override;
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
		bool get() const;
	/** @} */
	};
} }

#endif
