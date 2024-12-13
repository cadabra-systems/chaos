/**
 @file ChatEndpoint.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "ChatEndpoint.hpp"

namespace chaos {
	const bimap<std::string, chat_endpoint::message_service> chat_endpoint::service_map = {
		{
			{"xxx", chat_endpoint::message_service::invalid},
			{"cap", chat_endpoint::message_service::cap},
			{"wap", chat_endpoint::message_service::whatsapp},
			{"vbr", chat_endpoint::message_service::viber},
			{"tgm", chat_endpoint::message_service::telegram}
		},
		chat_endpoint::message_service::invalid
	};

	chat_endpoint::chat_endpoint(const std::string& token, const std::string& ident, chat_endpoint::message_service message_service)
	:
		_bot_token(token),
		_chat_token(ident),
		_message_service(message_service)
	{

	}

	chat_endpoint::chat_endpoint(const std::string& token, const std::string& ident)
	:
		_bot_token(token),
		_chat_token(ident),
		_message_service(message_service::invalid)
	{

	}

	chat_endpoint::chat_endpoint(const std::string& cns)
	:
		_message_service(message_service::invalid)
	{
		parse(cns);
	}

	chat_endpoint::chat_endpoint(const chat_endpoint& origin)
	:
		_bot_token(origin._bot_token),
		_chat_token(origin._chat_token),
		_message_service(origin._message_service)
	{

	}

	chat_endpoint::chat_endpoint(chat_endpoint&& origin)
	:
		_bot_token(std::move(origin._bot_token)),
		_chat_token(std::move(origin._chat_token)),
		_message_service(std::move(origin._message_service))
	{

	}

	chat_endpoint& chat_endpoint::operator=(const chat_endpoint& rhs)
	{
		_bot_token = rhs._bot_token;
		_chat_token = rhs._chat_token;
		_message_service = rhs._message_service;

		return *this;
	}

	chat_endpoint& chat_endpoint::operator=(chat_endpoint&& rhs)
	{
		_bot_token = std::move(rhs._bot_token);
		_chat_token = std::move(rhs._chat_token);
		_message_service = std::move(rhs._message_service);

		return *this;
	}

	bool chat_endpoint::operator<(const chat_endpoint& rhs) const
	{
		return _bot_token < rhs._bot_token;
	}

	void chat_endpoint::parse(const std::string& cns)
	{
		const std::string::size_type at_offset(cns.find_first_of('@'));
		const std::string::size_type dot_offset(cns.find_last_of('.'));

		if ((at_offset != std::string::npos && dot_offset != std::string::npos) || dot_offset < at_offset) {
			_bot_token = cns.substr(0, at_offset);
			_chat_token = cns.substr(at_offset + 1, dot_offset - at_offset - 1);
			_message_service = chat_endpoint::service_map.at(cns.substr(dot_offset + 1), true);
		} else {
			_message_service = message_service::invalid;
		}
	}

	void chat_endpoint::set(const std::string& bot_token, const std::string& chat_token, chat_endpoint::message_service message_service)
	{
		_bot_token = bot_token;
		_chat_token = chat_token;
		_message_service = message_service;
	}

	const std::string& chat_endpoint::bot_token() const
	{
		return _bot_token;
	}

	const std::string& chat_endpoint::chat_token() const
	{
		return _chat_token;
	}

	const chat_endpoint::message_service& chat_endpoint::service() const
	{
		return _message_service;
	}

	std::string chat_endpoint::cns() const
	{
		return _bot_token + "@" + _chat_token + "." + chat_endpoint::service_map.at(_message_service, true);
	}

	bool chat_endpoint::valid() const
	{
		return message_service::invalid != _message_service;
	}
}
