/**
 @file HTTPClient.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "HTTPClient.hpp"

#include "Log.hpp"
#include "Base64.hpp"

#include <algorithm>

namespace chaos {
	std::size_t http_client::request_buffer::copy(char* destination, std::size_t length)
	{
		length = std::min(data.length() - position, length);
		if (length > 0) {
			strncpy(destination, data.data() + position, length);
			position += length;
		}
		return length;
	}

	size_t http_client::request_callback(char* ptr, size_t size, size_t nmemb, void* request)
	{
		request_buffer* buffer(reinterpret_cast<request_buffer*>(request));
		const size_t real_size(size * nmemb);
		return (!buffer || real_size < 1) ? 0 : buffer->copy(ptr, real_size);
	}

	size_t http_client::response_callback(char* ptr, size_t size, size_t nmemb, void* response)
	{
		std::string* data(reinterpret_cast<std::string*>(response));

		const size_t real_size(size * nmemb);
		if (!data || real_size < 1) {
			return 0;
		}
		data->append(ptr, real_size);
		return real_size;
	}

	http_client::http_client(const uri& uri)
	:
		http_client(uri.host(), (uri.parameter<std::string>("secure", "false").compare("true") == 0))
	{

	}

	http_client::http_client(const std::string& hostname, bool secure)
	:
		_hostname(hostname),
		_secure(secure),
		_request_code(CURL_LAST),
		_response_code(0)
	{

	}

	http_client::http_client(http_client&& origin)
	:
		_hostname(std::move(origin._hostname)),
		_secure(std::move(origin._secure)),
		_request_code(origin._request_code),
		_response_code(origin._response_code),
		_response_body(std::move(origin._response_body)),
		_response_type(std::move(origin._response_type))
	{
		origin._request_code = CURL_LAST;
	}

	http_client::~http_client()
	{

	}

	bool http_client::perform(const std::shared_ptr<CURL>& curl, const std::string& path, const header_map& header2_map)
	{
		_request_code = CURL_LAST;
		_response_code = 0;
		_response_body.clear();
		_response_type.reset();

		if (!curl) {
			_request_code = CURLE_HTTP_RETURNED_ERROR;
			return false;
		}

		curl_easy_setopt(curl.get(), CURLOPT_URL, std::string((_secure ? std::string("https") : std::string("http")) + "://" + _hostname + "/" + path).data());
//		curl_easy_setopt(curl.get(), CURLOPT_PORT, _secure ? 443 : 80);

		std::unique_ptr<struct curl_slist, void(*)(struct curl_slist*)> header_slist(nullptr, curl_slist_free_all);
		for (const header_map& map : {header2_map, _header_map}) {
			if (!map.empty()) {
				for (const header_map::value_type& header : map) {
					header_slist.reset
					(
						curl_slist_append
						(
							header_slist.get(),
							std::string(header.first).append(": ").append(header.second).data()
						)
					);
				}
				if (!header_slist) {
					_request_code = CURLE_HTTP_RETURNED_ERROR;
					return false;
				}
				curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, header_slist.get());
			}
		}

		if ((_request_code = curl_easy_perform(curl.get())) != CURLE_OK) {
			return false;
		}
		curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &_response_code);
		if (_response_code != 200) {
			_request_code = CURLE_HTTP_RETURNED_ERROR;
			return false;
		}

		char* content_type_raw(nullptr);
		curl_easy_getinfo(curl.get(), CURLINFO_CONTENT_TYPE, &content_type_raw);
		std::string content_type(content_type_raw);
		std::transform(content_type.begin(), content_type.end(), content_type.begin(), ::tolower);
		_response_type.parse(content_type);
		return true;
	}

	bool http_client::send(const std::string& path, const mime& content_type, const std::string& content_data, send_mode mode)
	{
		request_buffer buffer{content_data, 0};
		const std::shared_ptr<CURL> curl(curl_easy_init(), curl_easy_cleanup);
		curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 15L);
		curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION, http_client::request_callback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, http_client::response_callback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &_response_body);
		if (send_mode::put == mode) {
			curl_easy_setopt(curl.get(), CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl.get(), CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(content_data.length()));
			curl_easy_setopt(curl.get(), CURLOPT_READDATA, &buffer);
		} else {
			curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE, static_cast<long>(content_data.length()));
			curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, content_data.data());
			if (send_mode::patch == mode) {
				curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, "PATCH");
			}
		}
		return perform
		(
			curl,
			path,
			{
				{"Content-Type", content_type.content_type()}
			}
		);
	}

	bool http_client::get(const std::string& path)
	{
		const std::shared_ptr<CURL> curl(curl_easy_init(), curl_easy_cleanup);
		curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 15L);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, http_client::response_callback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &_response_body);
		curl_easy_setopt(curl.get(), CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, nullptr);
		return perform(curl, path);
	}

	http_client::request_code http_client::get_error_code() const
	{
		return _request_code;
	}

	std::uint16_t http_client::get_response_code() const
	{
		return _response_code;
	}

	std::string http_client::get_error_string() const
	{
		return (CURLE_OK == _request_code) ? std::string() : std::string(curl_easy_strerror(_request_code));
	}

	std::string::size_type http_client::get_content_length() const
	{
		return _response_body.length();
	}

	const mime& http_client::get_content_type() const
	{
		return _response_type;
	}

	const std::string& http_client::get_content_body() const
	{
		return _response_body;
	}

	void http_client::authorize(const std::string& id, const std::string& password)
	{
		_header_map.emplace("Authorization", "Basic " + base64::encode(id + ":" + password));
	}

	void http_client::authorize(const std::string& token)
	{
		_header_map.emplace("Authorization", "Bearer  " + token);
	}

	bool http_client::is_ok() const
	{
		return CURLE_OK == _request_code && 200 == _response_code;
	}
}
