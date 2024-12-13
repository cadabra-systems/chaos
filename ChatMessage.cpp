/**
 @file ChatMessage.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "ChatMessage.hpp"

namespace chaos {
	chat_message::chat_message(const std::string& recipient, const std::string& body)
	:
		notification_message(body),

		_recipient_set({chaos::chat_endpoint(recipient)})
	{

	}

	chat_message::chat_message(const endpoint_set& recipient_set, const std::string& body)
	:
		notification_message(body),

		_recipient_set(recipient_set)
	{

	}

	chat_message::chat_message(const chat_endpoint& recipient, const std::string& body)
	:
		notification_message(body),

		_recipient_set({recipient})
	{

	}

	chat_message::chat_message(const std::string& body)
	:
		notification_message(body)
	{

	}

	chaos::flex chat_message::dump() const
	{
		chaos::flex recipient_array(chaos::flex::array());
		std::for_each(
						_recipient_set.begin(),
						_recipient_set.end(),
						[&recipient_array](const chat_endpoint& endpoint)
						{
							recipient_array.push_back(endpoint.cns());
						}
		);

		return {
				{"endpoint_tokens", recipient_array},
				{"message", get_body()},
				{"service", "chat"}
		};
	}

	bool chat_message::add_recipient(const chaos::chat_endpoint& recipient)
	{
		_recipient_set.insert(recipient);
		return true;
	}

	const chat_message::endpoint_set& chat_message::get_recipient_set() const
	{
		return _recipient_set;
	}
}
