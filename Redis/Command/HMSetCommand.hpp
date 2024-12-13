/**
 @file HMSetCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_HMSetCommand_hpp
#define Chaos_Redis_Command_HMSetCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	class hmset_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		hmset_command(std::string key, std::unordered_map<std::string, const std::string> map);
		hmset_command(std::string key, const std::string& member, const std::string& value);
		hmset_command(std::string key, const std::pair<const std::string, const std::string>& pair);
		hmset_command(hmset_command const&) = delete;
		hmset_command& operator=(hmset_command const&) = delete;
		virtual ~hmset_command() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::unordered_map<std::string, const std::string> _map;
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
