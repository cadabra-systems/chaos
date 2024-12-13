/**
 @file NotificationMessage.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "NotificationMessage.hpp"

namespace chaos {
	notification_message::notification_message(const std::string& body)
	:
		_uid(true),
		_body(body)
	{

	}

	notification_message::notification_message(notification_message&& origin)
	:
		_uid(std::move(origin._uid)),
		_body(std::move(origin._body))
	{

	}

	notification_message::notification_message(const notification_message& origin)
	:
		_uid(origin._uid),
		_body(origin._body)
	{

	}

	bool notification_message::invalidate(const chaos::flex& data) const
	{
		return true;
	}

	void notification_message::set_body(const std::string& body)
	{
		_body = body;
	}

	const uid& notification_message::get_uid() const
	{
		return _uid;
	}

	const std::string& notification_message::get_body() const
	{
		return _body;
	}
}
