/**
 * @file ExpireCommand.hpp
 * @date 2014-01-01
 * @copyright Cadabra Systems
 * @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Redis_Command_ExpireCommand_hpp
#define Chaos_Redis_Command_ExpireCommand_hpp

#include "../Command.hpp"

namespace chaos { namespace redis {
	/**
	 * @brief Set a timeout on key. After the timeout has expired, the key will automatically be deleted.
	 * A key with an associated timeout is often said to be volatile in Redis terminology.
	 * @return 0 if the timeout was not set. For example, if the key doesn't exist, or the operation skipped because of the provided arguments.
	 * @return 1 if the timeout was set.
	 */
	class expire_command : public function<void>
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class policy
		{
			none, /// < NX: Set expiry only when the key has no expiry
			update, /// < XX: Set expiry only when the key has an existing expiry
			greater, /// < GT: Set expiry only when the new expiry is greater than current one
			less /// < LT: Set expiry only when the new expiry is less than current one
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		expire_command(const std::string& key, const std::chrono::milliseconds& lifetime);
		virtual ~expire_command() override = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _key;
		const std::string _lifetime;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		virtual procedure::status execute(redisContext* context) override;
	/** @} */
	};
} }

#endif
