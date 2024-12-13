/**
 @file SMTPClient.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "SMTPClient.hpp"

#include "LogRegister.hpp"
#include "Log.hpp"
#include <curl/curl.h>

namespace chaos {
	smtp_client::security smtp_client::convert_security(const std::string& value)
	{
		if (value.empty()) {
			return chaos::smtp_client::security::none;
		}
		const std::string s(string::lowercased_string(value));
		if (s.compare("ssl") == 0) {
			return chaos::smtp_client::security::ssl;
		}
		if (s.compare("tls") == 0) {
			return chaos::smtp_client::security::tls;
		}
		if (s.compare("starttls") == 0) {
			return chaos::smtp_client::security::start_tls;
		}
		return chaos::smtp_client::security::none;
	}

	size_t smtp_client::payload_callback(char* ptr, size_t size, size_t nmemb, void* userp)
	{
		chaos::email_message::payload* payload(reinterpret_cast<chaos::email_message::payload*>(userp));

		if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1) || (payload->size == 0)) {
			return 0;
		}

		if ((nmemb * size) >= payload->data.size()) {
			payload->size = 0;
			return payload->data.copy(reinterpret_cast<char *>(ptr), payload->data.size());
		}

		return 0;
	}

	smtp_client::smtp_client(const chaos::uri& uri)
	:
		smtp_client(
					uri.host(),
					uri.port(),
					smtp_client::convert_security(uri.parameter<std::string>("secure", "")),
					uri.username(),
					uri.password()
		)
	{

	}

	smtp_client::smtp_client(const std::string& hostname, unsigned short port, security security, const std::string& username, const std::string& password)
	:
		_hostname(hostname),
		_port(port),
		_security(security),
		_username(username),
		_password(password)
	{

	}

	smtp_client::smtp_client(smtp_client&& origin)
	:
		_hostname(std::move(origin._hostname)),
		_port(std::move(origin._port)),
		_security(std::move(origin._security)),
		_username(std::move(origin._username)),
		_password(std::move(origin._password))
	{

	}

	smtp_client::~smtp_client()
	{

	}

	bool smtp_client::send(const email_message& message)
	{
		chaos::email_message::payload payload(message.make_payload());
		if (!payload) {
			return false;
		}

		const std::shared_ptr<CURL> curl(curl_easy_init(), curl_easy_cleanup);
		curl_easy_setopt(curl.get(), CURLOPT_URL, string::formatted_string("%s://%s:%d", ((_security == chaos::smtp_client::security::ssl || _security == chaos::smtp_client::security::tls) ? "smtps" : "smtp"), _hostname.c_str(), _port).c_str());
		if (!_username.empty()) {
			curl_easy_setopt(curl.get(), CURLOPT_USERNAME, _username.c_str());
		}
		if (!_password.empty()) {
			curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, _password.c_str());
		}

		if (security::start_tls == _security) {
			curl_easy_setopt(curl.get(), CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
		} else if (security::ssl == _security || security::tls == _security) {
			curl_easy_setopt(curl.get(), CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
//			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

			/** @brief If your server doesn't have a valid certificate, then you can disable
			 * part of the Transport Layer Security protection by setting the
			 * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
			 *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			 *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			 * That is, in general, a bad idea. It is still better than sending your
			 * authentication details in plain text though.  Instead, you should get
			 * the issuer certificate (or the host certificate if the certificate is
			 * self-signed) and add it to the set of certificates that are known to
			 * libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See docs/SSLCERTS
			 * for more information.
			 */
//			curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");
		} else {
			curl_easy_setopt(curl.get(), CURLOPT_USE_SSL, (long)CURLUSESSL_NONE);
		}

		/** @brief Note that this option isn't strictly required, omitting it will result
		 * in libcurl sending the MAIL FROM command with empty sender data. All
		 * autoresponses should have an empty reverse-path, and should be directed
		 * to the address in the reverse-path which triggered them. Otherwise,
		 * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
		 * details.
		 */
		curl_easy_setopt(curl.get(), CURLOPT_MAIL_FROM, message.get_sender().email().c_str());

		struct curl_slist* recipient_slist(nullptr);
		for (const chaos::email_message::address_set::value_type& address : message.get_recipient_set()) {
			recipient_slist = curl_slist_append(recipient_slist, address.email().data());
		}
		curl_easy_setopt(curl.get(), CURLOPT_MAIL_RCPT, recipient_slist);
		curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION, smtp_client::payload_callback);
		curl_easy_setopt(curl.get(), CURLOPT_READDATA, &payload);
		curl_easy_setopt(curl.get(), CURLOPT_UPLOAD, 1L);
//		curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 15L);
		const CURLcode retcode(curl_easy_perform(curl.get()));
		curl_slist_free_all(recipient_slist);
		if (retcode != CURLE_OK) {
			log_register<chaos::log>::error("SMTPClient(", this, ") > CURL error(", retcode, "): ", curl_easy_strerror(retcode));
			return false;
		}
		return true;
	}
}
