/**
 @file LoadZetCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_LoadZetCommand_hpp
#define Chaos_Redis_Command_LoadZetCommand_hpp

#include "../Command.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <limits>

namespace chaos { namespace redis {
	/**
	 * @brief ZRANGE key min max BYSCORE
	 *
	 * Returns all members whose score falls within [min_score, max_score].
	 * Pass std::numeric_limits<std::int64_t>::max() as max_score to query
	 * up to +inf (default).
	 *
	 * Typical use: discover all live cluster nodes for a given role.
	 *   min_score = now_ms,  max_score = +inf
	 */
	class load_zet_command : public function<std::vector<std::string>>
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit load_zet_command(const std::string& key, std::int64_t min_score, std::int64_t max_score = std::numeric_limits<std::int64_t>::max());
		virtual ~load_zet_command() override = default;
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
