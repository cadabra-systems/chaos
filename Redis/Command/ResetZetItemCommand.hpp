/**
 @file ResetZetItemCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_ResetZetItemCommand_hpp
#define Chaos_Redis_Command_ResetZetItemCommand_hpp

#include "../Command.hpp"

#include <string>
#include <cstdint>

namespace chaos { namespace redis {
	/**
	 * @brief ZADD XX score member
	 *
	 * Unconditionally updates an existing member's score.
	 * Has no effect if the member does not exist (XX semantics).
	 * Unlike update_zet_item_command, no GT/LT guard is applied — the score
	 * is always overwritten.
	 */
	class reset_zet_item_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		reset_zet_item_command(const std::string& key, std::int64_t score, const std::string& member);
		virtual ~reset_zet_item_command() override = default;
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
