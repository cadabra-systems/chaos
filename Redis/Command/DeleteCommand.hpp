/**
 @file DeleteCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_DeleteCommand_hpp
#define Chaos_Redis_Command_DeleteCommand_hpp

#include "../Command.hpp"

#include <string>
#include <vector>

namespace chaos { namespace redis {
	/**
	 * @brief DEL key [key ...]
	 *
	 * Deletes one or more keys. Returns the number of keys that were removed.
	 * Keys that do not exist are ignored and not counted.
	 */
	class delete_command : public function<std::int32_t>
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit delete_command(std::string key);
		explicit delete_command(std::vector<std::string> key_vector);
		virtual ~delete_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::vector<std::string> _key_vector;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
