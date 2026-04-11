/**
 @file UpdateZetItemCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_UpdateZetItemCommand_hpp
#define Chaos_Redis_Command_UpdateZetItemCommand_hpp

#include "../Command.hpp"

#include <string>
#include <cstdint>

namespace chaos { namespace redis {
	/**
	 * @brief ZADD XX [GT|LT] score member
	 *
	 * Updates an existing member's score. Has no effect if the member
	 * does not exist (XX semantics).
	 *
	 * The sign of score encodes the comparison direction:
	 *   score > 0  →  ZADD XX GT  score   member  (update only if new score is greater)
	 *   score < 0  →  ZADD XX LT  |score| member  (update only if new score is less)
	 *
	 * Typical use: cluster node TTL renewal (always positive → GT).
	 */
	class update_zet_item_command : public function<bool>
	{
	/** @name Constructors */
	/** @{ */
	public:
		update_zet_item_command(const std::string& key, std::int64_t score, const std::string& member);
		virtual ~update_zet_item_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::string _score;
		const bool _gl;
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
