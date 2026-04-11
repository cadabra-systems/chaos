/**
 @file PruneZetCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_PruneZetCommand_hpp
#define Chaos_Redis_Command_PruneZetCommand_hpp

#include "../Command.hpp"

#include <string>
#include <cstdint>
#include <limits>

namespace chaos { namespace redis {
	/**
	 * @brief ZREMRANGEBYSCORE key min max
	 *
	 * Removes all members whose score falls within [min_score, max_score].
	 * Use std::numeric_limits<std::int64_t>::min() for -inf (default).
	 * Use std::numeric_limits<std::int64_t>::max() for +inf.
	 *
	 * Typical use: GC of expired cluster nodes.
	 *   min_score = -inf (default),  max_score = now_ms - 1
	 */
	class prune_zet_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit prune_zet_command(const std::string& key, std::int64_t min_score = std::numeric_limits<std::int64_t>::min(), std::int64_t max_score = std::numeric_limits<std::int64_t>::max());
		virtual ~prune_zet_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::int64_t _min_score;
		const std::int64_t _max_score;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
