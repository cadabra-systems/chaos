/**
 * @file PersistCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_PersistCommand_hpp
#define Chaos_Redis_Command_PersistCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	/**
	 * @brief Remove the existing timeout on key,
	 * turning the key from volatile (a key with an expire set) to persistent (a key that will never expire as no timeout is associated).
	 * @retval 0 if key does not exist or does not have an associated timeout.
	 * @retval 1 if the timeout has been removed.
	 */
	class persist_command : public function<void>
	{
	/** @name Constructors */
	/** @{ */
	public:
		persist_command(const std::string& key);
		virtual ~persist_command() override = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
