/**
 @file ShortMessage.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "ShortMessage.hpp"

namespace chaos {
	short_message::short_message(const std::uint64_t& recipient, const std::string& body)
	:
		notification_message(body),

		_recipient_set({chaos::phone_number(recipient)})
	{

	}

	short_message::short_message(const std::set<std::uint64_t>& recipient_set, const std::string& body)
	:
		notification_message(body)
	{
		number_set& set(_recipient_set);
		std::for_each(
						recipient_set.cbegin(),
						recipient_set.cend(),
						[&set](const std::uint64_t& r)
						{
							set.emplace(r);
						}
		);
	}

	short_message::short_message(const number_set& recipient_set, const std::string& body)
	:
		notification_message(body),

		_recipient_set(recipient_set)
	{

	}

	short_message::short_message(const phone_number& recipient, const std::string& body)
	:
		notification_message(body),

		_recipient_set({recipient})
	{

	}

	short_message::short_message(const std::string& body)
	:
		notification_message(body)
	{

	}

	short_message::short_message(short_message&& origin)
	:
		notification_message(std::move(origin)),

		_recipient_set(std::move(origin._recipient_set))
	{

	}

	short_message::short_message(const short_message& origin)
	:
		notification_message(origin),

		_recipient_set(origin._recipient_set)
	{

	}

	chaos::flex short_message::dump() const
	{
		chaos::flex recipient_array(chaos::flex::array());
		std::for_each(
						_recipient_set.begin(),
						_recipient_set.end(),
						[&recipient_array](const phone_number& number)
						{
							recipient_array.push_back(std::to_string(number.e164()));
						}
		);

		return {
				{"phone_numbers", recipient_array},
				{"message", get_body()},
				{"service", "sms"}
		};
	}

	bool short_message::add_recipient(const chaos::phone_number& recipient)
	{
		_recipient_set.insert(recipient);
		return true;
	}

	const short_message::number_set& short_message::get_recipient_set() const
	{
		return _recipient_set;
	}
}
