/**
 @file RemoveZetItemCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_RemoveZetItemCommand_hpp
#define Chaos_Redis_Command_RemoveZetItemCommand_hpp

#include "../Command.hpp"

#include <string>

namespace chaos { namespace redis {
	/**
	 * @brief ZREM key member
	 *
	 * Removes a specific member from a sorted set.
	 * Typical use: explicit node deregistration on graceful shutdown.
	 */
	class remove_zet_item_command : public function<bool>
	{
	/** @name Constructors */
	/** @{ */
	public:
		remove_zet_item_command(const std::string& key, const std::string& member);
		virtual ~remove_zet_item_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::string _member;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
