/**
 @file EmplaceZetItemCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_EmplaceZetItemCommand_hpp
#define Chaos_Redis_Command_EmplaceZetItemCommand_hpp

#include "../Command.hpp"

#include <string>
#include <cstdint>

namespace chaos { namespace redis {
	/**
	 * @brief ZADD NX score member
	 *
	 * Adds a new member to a sorted set only if it does not already exist.
	 * If the member exists the command has no effect (NX semantics).
	 */
	class emplace_zet_item_command : public function<bool>
	{
	/** @name Constructors */
	/** @{ */
	public:
		emplace_zet_item_command(const std::string& key, std::int64_t score, const std::string& member);
		virtual ~emplace_zet_item_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::string _score;
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
