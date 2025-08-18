/**
 @file FiscusClient.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "FiscusClient.hpp"

#include "Log.hpp"
#include "../Flex/UID.hpp"
#include "../Flex/URI.hpp"

namespace chaos { namespace cws {
	fiscus_client::fiscus_client(const uri& uri)
	:
		fiscus_client
		(
			uri.host(),
			uri.username(),
			(uri.parameter<std::string>("secure", "false").compare("true") == 0)
		)
	{

	}

	fiscus_client::fiscus_client(const std::string& hostname, const std::string& shop_key, bool secure)
	:
		http_client(hostname, secure),

		_shop_key(shop_key)
	{

	}

	fiscus_client::fiscus_client(fiscus_client&& origin)
	:
		http_client(std::move(origin)),

		_shop_key(std::move(origin._shop_key))
	{

	}

	fiscus_client::~fiscus_client()
	{

	}

	fiscus_client::cheque fiscus_client::sign(cheque&& origin) const
	{
		if (!origin.guid.is_zero()) {
			origin.endpoint = string::formatted_string("fiscus://%s@%s?secure=%s", _shop_key.data(), _hostname.data(), _secure ? "true" : "false");
		}
		return std::move(origin);
	}

	std::list<fiscus_client::cheque> fiscus_client::sign(std::list<cheque>&& origin) const
	{
		for (cheque& c : origin) {
			c = sign(std::move(c));
		}
		return std::move(origin);
	}

	bool fiscus_client::search
	(
		bool verbose,
		const std::set<std::string>& sku_list,
		const std::set<email_address>& email_address_list,
		const std::set<phone_number>& phone_number_list,
		const tribool& punch,
		const tribool& refund,
		const std::pair<std::chrono::time_point<std::chrono::system_clock>,
		std::chrono::time_point<std::chrono::system_clock>>& timeframe
	)
	{
		flex email_address_array(flex::array());
		std::for_each
		(
			email_address_list.cbegin(), email_address_list.cend(),
			[&email_address_array](const email_address& address)
			{
				email_address_array.emplace_back(address.address());
			}
		);

		flex phone_number_array(flex::array());
		std::for_each
		(
			phone_number_list.cbegin(), phone_number_list.cend(),
			[&phone_number_array](const phone_number& number)
			{
				phone_number_array.emplace_back(number.e164());
			}
		);

		flex flex_request{{"verbose", verbose}, {"skus", sku_list}};
		if (!phone_number_list.empty()) {
			flex_request["phone_numbers"] = phone_number_array;
		}
		if (!email_address_list.empty()) {
			flex_request["email_addresses"] = email_address_array;
		}
		if (!punch.is_indeterminate()) {
			flex_request["punch"] = punch.is_on() ? true : false;
		}
		if (!refund.is_indeterminate()) {
			flex_request["refund"] = refund.is_on() ? true : false;
		}
		const std::pair<int, int> interval
		{
			std::chrono::duration_cast<std::chrono::seconds>(timeframe.first.time_since_epoch()).count(),
			std::chrono::duration_cast<std::chrono::seconds>(timeframe.second.time_since_epoch()).count()
		};
		if (interval.first == 0) {
			if (interval.second != 0) {
				flex_request["till"] = interval.second;
			}
		} else if (interval.second == 0) {
			if (interval.first != 0) {
				flex_request["since"] = interval.first;
			}
		} else if (interval.second > interval.first) {
			flex_request["till"] = interval.second;
			flex_request["since"] = interval.first;
		} else {
			flex_request["till"] = interval.first;
			flex_request["since"] = interval.second;
		}

		if (!http_client::send("seek/" + _shop_key, {"application/json"}, flex::to_json(flex_request))) {
			if (get_error_code() == CURLE_HTTP_RETURNED_ERROR) {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the seek an error has occurred on the remote side");
			} else {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the seek an error has occurred: ", get_error_string());
			}
			return false;
		}
		return http_client::get_content_type().kind(mime::media_type::application, "json");
	}

	uid fiscus_client::record(const std::string& engine, const flex& payload)
	{
		if (!http_client::send("record/" + _shop_key + "/" + engine, {"application/json"}, flex::to_json(payload), http_client::send_mode::post)) {
			if (get_error_code() == CURLE_HTTP_RETURNED_ERROR) {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the record an error has occurred on the remote side");
			} else {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the record an error has occurred: ", get_error_string());
			}
			return {};
		}
		return get_content_type().kind(mime::media_type::application, "json") ? uid{flex::from_json(get_content_body()).value("guid", "")} : uid{};
	}

	std::list<uid> fiscus_client::seek
	(
		const std::set<std::string>& sku_list,
		const std::set<email_address>& email_address_list,
		const std::set<phone_number>& phone_number_list,
		const tribool& punch,
		const tribool& refund,
		const std::pair<std::chrono::time_point<std::chrono::system_clock>,
		std::chrono::time_point<std::chrono::system_clock>>& timeframe
	)
	{
		if (!search(false, sku_list, email_address_list, phone_number_list, punch, refund, timeframe)) {
			return {};
		}
		const flex payload(flex::from_json(get_content_body()));
		return payload.is_object() ? payload.value("response", std::list<uid>{}) : std::list<uid>{};
	}

	std::list<fiscus_client::cheque> fiscus_client::scan
	(
		const std::set<std::string>& sku_list,
		const std::set<email_address>& email_address_list,
		const std::set<phone_number>& phone_number_list,
		const tribool& punch,
		const tribool& refund,
		const std::pair<std::chrono::time_point<std::chrono::system_clock>,
		std::chrono::time_point<std::chrono::system_clock>>& timeframe
	)
	{
		if (!search(true, sku_list, email_address_list, phone_number_list, punch, refund, timeframe)) {
			return {};
		}
		const flex payload(flex::from_json(get_content_body()));
		return payload.is_object() ? sign(payload.value("response", std::list<fiscus_client::cheque>{})) : std::list<fiscus_client::cheque>{};
	}

	fiscus_client::cheque fiscus_client::check(const uid& guid)
	{
		if (!http_client::get("check/" + _shop_key + "/" + guid.make_string() + "/json")) {
			if (get_error_code() == CURLE_HTTP_RETURNED_ERROR) {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the check an error has occurred on the remote side");
			} else {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the check an error has occurred: ", get_error_string());
			}
			return {};
		} else if (!http_client::get_content_type().kind(mime::media_type::application, "json")) {
			return {};
		}
		const flex payload(flex::from_json(get_content_body()));
		return payload.is_object() ? sign(payload.get<cheque>()) : cheque{};
	}

	bool fiscus_client::punch(const uid& guid)
	{
		if (!http_client::get("punch/" + _shop_key + "/" + guid.make_string())) {
			if (get_error_code() == CURLE_HTTP_RETURNED_ERROR) {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the punch an error has occurred on the remote side");
			} else {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the punch an error has occurred: ", get_error_string());
			}
			return false;
		}
		return http_client::is_ok();
	}

	bool fiscus_client::refund(const uid& guid)
	{
		if (!http_client::get("refund/" + _shop_key + "/" + guid.make_string())) {
			if (get_error_code() == CURLE_HTTP_RETURNED_ERROR) {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the refund an error has occurred on the remote side");
			} else {
				log_register<cws::log>::error("FiscusClient(", this, ") > During the refund an error has occurred: ", get_error_string());
			}
			return false;
		}
		return http_client::is_ok();
	}

	std::set<std::string> fiscus_client::cheque::make_sku_set() const
	{
		std::set<std::string> retval;
		std::for_each(item_list.cbegin(), item_list.cend(), [&retval](const item& i) { retval.emplace(i.sku); } );
		return retval;
	}

	bool fiscus_client::cheque::is_punched() const
	{
		return punch.time_since_epoch().count() > 0;
	}

	bool fiscus_client::cheque::has_refund() const
	{
		return refund.time_since_epoch().count() > 0;
	}

	void to_flex(flex& output, const fiscus_client::cheque::item& input)
	{
		output["sku"] = input.sku;
		output["quantity"] = input.quantity;
		output["price"] = input.price;
		output["name"] = input.name;
	}
	void from_flex(const flex& input, fiscus_client::cheque::item& output)
	{
		output.sku = input.value("sku", "");
		output.name = input.value("name", "");
		output.quantity = input.value("quantity", 0.0);
		output.price = input.value("price", 0);
		if (0 == output.price) { /// < failover
			 output.price = input.value("cost", 0);
		}
	}

	void to_flex(flex& output, const fiscus_client::cheque& input)
	{
		output["endpoint"] = input.endpoint;
		output["guid"] = input.guid;
		output["issue"] = input.issue.time_since_epoch().count();
		output["refund"] = input.refund.time_since_epoch().count();
		output["punch"] = input.punch.time_since_epoch().count();
		output["customer_name"] = input.customer_email_address.name();
		output["customer_email_address"] = input.customer_email_address.address();
		output["customer_phone_number"] = input.customer_phone_number.e164();
		output["currency"] = money::currency_code(input.currency, true);
		output["web_engine"] = input.web_engine;
		output["payment_system"] = input.payment_system;
		output["items"] = input.item_list;
	}
	void from_flex(const flex& input, fiscus_client::cheque& output)
	{
		output.endpoint = input.value("endpoint", uri{});
		output.guid = input.value("guid", uid{});
		output.issue = std::chrono::system_clock::time_point(std::chrono::seconds(input.value("issue", 0)));
		output.refund = std::chrono::system_clock::time_point(std::chrono::seconds(input.value("refund", 0)));
		output.punch = std::chrono::system_clock::time_point(std::chrono::seconds(input.value("punch", 0)));
		output.customer_email_address = email_address(input.value("customer_email_address", ""), input.value("customer_name", ""));
		output.customer_phone_number = phone_number(input.value("customer_phone_number", static_cast<std::int64_t>(0)));
		output.currency = money::currency_map.at(string::uppercased_string(input.value("currency", "XXX")), true);
		output.web_engine = input.value("web_engine", "");
		output.payment_system = input.value("payment_system", "");
		output.item_list = input.value("items", std::list<fiscus_client::cheque::item>{});
	}
} }
