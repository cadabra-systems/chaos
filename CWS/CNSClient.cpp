/**
 @file CNSClient.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "CNSClient.hpp"

#include "Log.hpp"

namespace chaos { namespace cws {
	cns_client::cns_client(const uri& uri)
	:
		cns_client(
					uri.host(),
					uri.username(),
					(uri.parameter<std::string>("secure", "false").compare("true") == 0),
					(
						uri.parameter<std::string>("deffer", "off").compare("on") == 0
						||
						uri.parameter<std::string>("deffer", "false").compare("true") == 0
					)
		)
	{
		_batch_size = uri.parameter<std::size_t>("batch_size", 0);
	}

	cns_client::cns_client(const std::string& hostname, const std::string& access_key, bool secure, bool deffer_mode)
	:
		web_api(hostname, secure),

		_access_key(access_key),
		_deffer_mode(deffer_mode),
		_batch_size(0)
	{

	}

	cns_client::cns_client(cns_client&& origin)
	:
		web_api(std::move(origin)),

		_access_key(std::move(origin._access_key)),
		_deffer_mode(std::move(origin._deffer_mode)),
		_batch_size(std::move(origin._batch_size))
	{

	}

	cns_client::~cns_client()
	{

	}

	bool cns_client::send(const std::list<std::reference_wrapper<const notification_message>>& message_list, trap* trap_set)
	{
		if (message_list.empty()) {
			return true;
		}

		chaos::flex notification_array(chaos::flex::array());
		std::for_each(
						message_list.cbegin(),
						message_list.cend(),
						[&notification_array](const std::reference_wrapper<const notification_message>& message)
						{
							notification_array.emplace_back(message.get().dump());
						}
		);
		/// @todo batchsize
		if (!web_api::send("", {"application/json"}, chaos::flex::to_json({{"proto_key", 1}, {"access_key", _access_key}, {"notifications", notification_array}}), _deffer_mode)) {
			if (get_error_code() == CURLE_HTTP_RETURNED_ERROR) {
				chaos::log_register<cws::log>::error("CNSClient(", this, ") > During the request an error has occurred on the remote side");
			} else {
				chaos::log_register<cws::log>::error("CNSClient(", this, ") > During the request an error has occurred: ", get_error_string());
			}
			return false;
		}
		const mime content_type(web_api::get_content_type());
		if (!content_type.kind(mime::media_type::application, "json")) {
			return true;
		} else if (get_content_length() <= 0) {
			chaos::log_register<cws::log>::warning("CNSClient(", this, ") > Content-type is JSON, but nothing really came back");
			return true;
		}

		flex flex_response;
		try {
			flex_response = chaos::flex::from_json(get_content_body());
		} catch (const flex::exception& e) {
			chaos::log_register<cws::log>::error("CNSClient(", this, ") > Flex error: ", e.what(), " in response: ", get_content_body());
			return true;
		}

		chaos::flex::const_iterator na(flex_response.find("notifications"));
		if (flex_response.cend() == na || !na->is_array()) {
			return false;
		} else if (!_deffer_mode) {
			chaos::flex::const_iterator n(na->begin());
			std::for_each(
							message_list.cbegin(),
							message_list.cend(),
							[&n, &na](const std::reference_wrapper<const notification_message>& notification)
							{
								if (na->cend() != n) {
									notification.get().invalidate(*(n++));
								}
							}
			);
		}
		if (trap_set) {
			std::for_each(
							na->cbegin(),
							na->cend(),
							[trap_set](const flex& notification)
							{
//								short_message::trap(notification, trap_set->get().phone_number_set);
								push_message::trap(notification, trap_set->device_token_set);
//								email_message::trap(notification, trap_set->get().email_address_set);
							}
			);
		}
		return true;
	}

	bool cns_client::send(const notification_message& message)
	{
		return send({std::cref(message)}, nullptr);
	}

	bool cns_client::send(const std::list<std::reference_wrapper<const notification_message>>& message_list)
	{
		return send(message_list, nullptr);
	}

	bool cns_client::send(const std::list<std::reference_wrapper<const notification_message>>& message_list, trap& trap_set)
	{
		return send(message_list, &trap_set);
	}

	bool cns_client::is_deffered() const
	{
		return _deffer_mode;
	}
} }
