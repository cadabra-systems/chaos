/**
 @file ScanCommand.hpp
 @date 2026-04-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Redis_Command_ScanCommand_hpp
#define Chaos_Redis_Command_ScanCommand_hpp

#include "../Command.hpp"

#include <string>

namespace chaos { namespace redis {
	/**
	 * @brief SCAN cursor MATCH pattern COUNT hint → std::vector<std::string>
	 *
	 * Each execute() issues exactly one SCAN command using the internally
	 * tracked cursor. Cursor management and string accumulation are handled
	 * by cursor<std::string>. Use sync_connection::loop<> to iterate all pages.
	 */
	class scan_command : public cursor<std::string>
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit scan_command(const std::string& pattern, std::size_t count = 100);
		virtual ~scan_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _pattern;
		const std::string _count;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
