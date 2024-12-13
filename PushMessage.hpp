#/**
@file PushMessage.hpp
@date 2014-01-01
@copyright Cadabra Systems
@author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_PushMessage_hpp
#define Chaos_PushMessage_hpp

#include "NotificationMessage.hpp"

#include "DeviceToken.hpp"
#include "Flex.hpp"

#include <set>
#include <string>

namespace chaos {
	class push_message : public notification_message
	{
	/** @name Aliases */
	/** @{ */
	public:
		using token_set = std::set<device_token>;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static bool trap(const flex& data, token_set& invalid_set);
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		push_message(token_set&& recipient_set, const std::string& title, const std::string& body, const chaos::flex& payload = {}, const std::string& sound = "default");
		push_message(const token_set& recipient_set, const std::string& title, const std::string& body, const chaos::flex& payload = {}, const std::string& sound = "default");
		push_message(const device_token& recipient, const std::string& title, const std::string& body, const chaos::flex& payload = {});
		push_message(const std::string& title, const std::string& body, const chaos::flex& payload = {});
		push_message(push_message&& origin);
		push_message(const push_message& origin);
		virtual ~push_message() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		token_set _recipient_set;
		mutable token_set _invalid_set;
		std::string _title;
		std::string _sound;
		flex _payload;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual chaos::flex dump() const override;
		virtual bool invalidate(const chaos::flex& data) const override;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set_payload(const flex& payload);
		void set_sound(const std::string& sound);
		bool set_invalid(const chaos::device_token& recipient);
		bool add_recipient(const device_token& recipient);
		bool add_invalid(const chaos::device_token& recipient) const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const token_set& get_recipient_set() const;
		const token_set& get_invalid_set() const;
		const std::string& get_title() const;
		const std::string& get_sound() const;
		const flex& get_payload() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool any_recipient() const;
		bool any_invalid() const;
	/** @} */
	};
}

#endif
