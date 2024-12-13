#/**
@file ChatMessage.hpp
@date 2014-01-01
@copyright Cadabra Systems
@author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ChatMessage_hpp
#define Chaos_ChatMessage_hpp

#include "NotificationMessage.hpp"

#include "ChatEndpoint.hpp"
#include "Flex.hpp"

#include <set>
#include <string>
#include <cstdint>

namespace chaos {
	class chat_message : public notification_message
	{
	/** @name Aliases */
	/** @{ */
	public:
		using endpoint_set = std::set<chat_endpoint>;
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		chat_message(const std::string& recipient, const std::string& body);
		chat_message(const std::set<chat_endpoint>& recipient_set, const std::string& body);
		chat_message(const chat_endpoint& recipient, const std::string& body);
		chat_message(const std::string& body);
		virtual ~chat_message() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		endpoint_set _recipient_set;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual chaos::flex dump() const override;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		bool add_recipient(const chat_endpoint& recipient);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const endpoint_set& get_recipient_set() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
