/**
 @file SetCommand.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_SetCommand_hpp
#define Chaos_Redis_Command_SetCommand_hpp

#include "../Command.hpp"

#include <chrono>

namespace chaos { namespace redis {
	template<typename T>
	/**
	 SET key value [EX seconds] [PX milliseconds] [NX|XX]
	 EX seconds -- Set the specified expire time, in seconds.
	 PX milliseconds -- Set the specified expire time, in milliseconds.
	 NX -- Only set the key if it does not already exist.
	 XX -- Only set the key if it already exist.
	 */
	class set_command : public command
	{
	/** @name Classes */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		set_command(std::string key, T value, set_mode mode = set_mode::undefined, std::chrono::seconds expiration = std::chrono::seconds(0))
		:
			_key(key),
			_value(value),
			_mode(mode),
			_expiration(expiration)
		{
			
		}
		set_command(set_command const&) = delete;
		set_command& operator=(set_command const&) = delete;
		virtual ~set_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const T _value;
		const set_mode _mode;
		const std::chrono::seconds _expiration;
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
			std::list<std::string> option_list;
			if (_expiration.count() > 0) {
				option_list.push_back(std::string("EX " + std::to_string(_expiration.count())));
			}
			if (_mode != set_mode::undefined) {
				option_list.push_back({static_cast<char>(_mode), 'X'});
			}
			
			return command_line("SET", _key, _value, option_list);
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
			if (get_reply() != reply::type::string) {
				return false;
			}
			
			return get_reply().get_string().compare("OK") == 0;
		}
	/** @} */
	};
} }

#endif
