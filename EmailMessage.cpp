/**
 @file EmailMessage.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "EmailMessage.hpp"

#include "UID.hpp"
#include "Base64.hpp"

#include <chrono>

namespace chaos {
	email_message::email_message(const email_address& sender, const email_address& recipient, const std::string& subject, const std::string& body)
	:
		notification_message(body),

		_sender(sender),
		_recipient_set({recipient}),
		_subject(subject)
	{

	}

	email_message::email_message(const email_address& recipient, const std::string& subject, const std::string& body)
	:
		notification_message(body),

		_recipient_set({recipient}),
		_subject(subject)
	{

	}

	email_message::email_message(const std::string& subject, const std::string& body)
	:
		notification_message(body),

		_subject(subject)
	{

	}

	email_message::email_message(email_message&& origin)
	:
		notification_message(std::move(origin)),

		_sender(origin._sender),
		_recipient_set(std::move(origin._recipient_set)),
		_subject(std::move(origin._subject))
	{

	}

	email_message::email_message(const email_message& origin)
	:
		notification_message(origin),

		_sender(origin._sender),
		_recipient_set(origin._recipient_set),
		_subject(origin._subject)
	{

	}

	email_message::payload email_message::make_payload(const std::string& message_id_suffix) const
	{
		address_set::const_iterator to(_recipient_set.begin());
		if (_recipient_set.cend() == to) {
			return {};
		}

		const chaos::uid guid(true);
		const std::chrono::time_point<std::chrono::system_clock> time_point(std::chrono::system_clock::now());
		const std::time_t time(std::chrono::system_clock::to_time_t(time_point));
		std::stringstream stream;
		stream << "Date: " << std::put_time(std::localtime(&time), "%c %z")  << "\r\n"
				<< "To: " << to->rfc() << "\r\n"
				<< "From: " << _sender.rfc() << "\r\n";
		while (++to != _recipient_set.cend()) {
				stream
				<< "Cc: " << to->rfc() << "\r\n";
		}
		stream << "Message-ID: <" << guid.make_string() << "@" << (message_id_suffix.empty() ? _sender.hostname() : message_id_suffix) << ">\r\n"
				<< "Content-Type: text/plain; charset=UTF-8\r\n"
				<< "X-Mailer: cadabra.chaos" << "\r\n"
				<< "Subject: " << "=?UTF-8?B?" << chaos::base64::encode(_subject) << "?=\r\n"
				<< "\r\n" /* empty line to divide headers from body, see RFC5322 */
				<< get_body() + "\r\n"
				<< "\r\n"
//				<< "It could be a lot of lines, could be MIME encoded, whatever.\r\n"
//				<< "Check RFC5322.\r\n"
		;

		return payload(stream);
	}

	chaos::flex email_message::dump() const
	{
		chaos::flex recipient_array(chaos::flex::array());
		std::for_each(
						_recipient_set.begin(),
						_recipient_set.end(),
						[&recipient_array](const email_address& address)
						{
							recipient_array.push_back(
														{
															{"address", address.address()},
															{"name", address.name()}
														}
							);
						}
		);

		return {
					{"sender_address", {{"address", _sender.address()}, {"name", _sender.name()}}},
					{"recipient_addresses", recipient_array},
					{"subject", _subject},
					{"body", get_body()},
					{"service", "email"}
		};
	}

	void email_message::set_sender(const email_address& sender)
	{
		/// @todo check valid
		_sender = sender;
	}

	void email_message::add_recipient(const email_address& recipient)
	{
		/// @todo check valid
		_recipient_set.insert(recipient);
	}

	void email_message::set_subject(const std::string& subject)
	{
		_subject = subject;
	}

	const email_address& email_message::get_sender() const
	{
		return _sender;
	}

	const email_message::address_set& email_message::get_recipient_set() const
	{
		return _recipient_set;
	}

	const std::string& email_message::get_subject() const
	{
		return _subject;
	}

	bool email_message::is_complete() const
	{
		return true;
	}

	email_message::payload::payload(const std::stringstream& stream)
	:
		data(stream.str()),
		size(data.size())
	{
	}

	email_message::payload::payload()
	:
		data(""),
		size(0)
	{

	}

	bool email_message::payload::operator!() const
	{
		return size <= 0;
	}
}
