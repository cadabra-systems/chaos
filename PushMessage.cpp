/**
 @file PushMessage.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "PushMessage.hpp"

namespace chaos {
	bool push_message::trap(const flex& data, token_set& invalid_set)
	{
		chaos::flex::const_iterator a(data.find("invalid_device_tokens"));
		if (data.cend() == a || !a->is_array()) {
			return false;
		}
		std::for_each(
						a->cbegin(),
						a->cend(),
						[&invalid_set](const chaos::flex& value)
						{
							if (value.is_string()) {
								device_token token(value.get<std::string>());
								if (token.valid()) {
									invalid_set.emplace(std::move(token));
								}
							}
						}
		);
		return true;
	}

	push_message::push_message(token_set&& recipient_set, const std::string& title, const std::string& body, const chaos::flex& payload, const std::string& sound)
	:
		notification_message(body),

		_recipient_set(std::move(recipient_set)),
		_title(title),
		_sound(sound),
		_payload(payload)
	{


	}

	push_message::push_message(const token_set& recipient_set, const std::string& title, const std::string& body, const chaos::flex& payload, const std::string& sound)
	:
		notification_message(body),

		_recipient_set(recipient_set),
		_title(title),
		_sound(sound),
		_payload(payload)
	{


	}

	push_message::push_message(const chaos::device_token& recipient, const std::string& title, const std::string& body, const chaos::flex& payload)
	:
		notification_message(body),

		_recipient_set({recipient}),
		_title(title),
		_sound("default"),
		_payload(payload)
	{

	}

	push_message::push_message(const std::string& title, const std::string& body, const chaos::flex& payload)
	:
		notification_message(body),

		_title(title),
		_sound("default"),
		_payload(payload)
	{

	}

	push_message::push_message(push_message&& origin)
	:
		notification_message(std::move(origin)),

		_recipient_set(std::move(origin._recipient_set)),
		_title(std::move(origin._title)),
		_sound(std::move(origin._sound)),
		_payload(std::move(origin._payload))
	{

	}

	push_message::push_message(const push_message& origin)
	:
		notification_message(origin),

		_recipient_set(origin._recipient_set),
		_title(origin._title),
		_sound(origin._sound),
		_payload(origin._payload)
	{

	}

	chaos::flex push_message::dump() const
	{
		chaos::flex recipient_array(chaos::flex::array());
		std::for_each(
						_recipient_set.begin(),
						_recipient_set.end(),
						[&recipient_array](const device_token& token)
						{
							recipient_array.push_back(token.cns());
						}
		);

		return {
				{"device_tokens", recipient_array},
				{"message", get_body()},
				{"title", _title},
				{"sound", _sound},
				{"payload", _payload},
				{"service", "push"}
		};
	}

	bool push_message::invalidate(const chaos::flex& data) const
	{
		return push_message::trap(data, _invalid_set);
	}

	void push_message::set_payload(const flex& payload)
	{
		_payload = payload;
	}

	void push_message::set_sound(const std::string& sound)
	{
		_sound = sound;
	}

	bool push_message::set_invalid(const chaos::device_token& recipient)
	{
		if (!recipient.valid()) {
			return false;
		} else if (_recipient_set.erase(recipient) != 1) {
			return false;
		}
		_invalid_set.insert(recipient);
		return true;
	}

	bool push_message::add_recipient(const chaos::device_token& recipient)
	{
		if (!recipient.valid()) {
			return false;
		}
		_recipient_set.insert(recipient);
		return true;
	}

	bool push_message::add_invalid(const chaos::device_token& recipient) const
	{
		if (!recipient.valid()) {
			return false;
		}
		_invalid_set.insert(recipient);
		return true;
	}

	const push_message::token_set& push_message::get_recipient_set() const
	{
		return _recipient_set;
	}

	const push_message::token_set& push_message::get_invalid_set() const
	{
		return _invalid_set;
	}

	const std::string& push_message::get_title() const
	{
		return _title;
	}

	const std::string& push_message::get_sound() const
	{
		return _sound;
	}

	const chaos::flex& push_message::get_payload() const
	{
		return _payload;
	}

	bool push_message::any_recipient() const
	{
		return !_recipient_set.empty();
	}

	bool push_message::any_invalid() const
	{
		return !_invalid_set.empty();
	}
}
