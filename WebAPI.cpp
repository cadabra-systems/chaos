/**
 @file WebAPI.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "WebAPI.hpp"

#include "Log.hpp"

#include <algorithm>

namespace chaos {
	std::size_t web_api::request_buffer::copy(char* destination, std::size_t length)
	{
		length = std::min(data.length() - position, length);
		if (length > 0) {
			strncpy(destination, data.data() + position, length);
			position += length;
		}
		return length;
	}

	size_t web_api::request_callback(char* ptr, size_t size, size_t nmemb, void* request)
	{
	  request_buffer* buffer(reinterpret_cast<request_buffer*>(request));

	  const size_t real_size(size * nmemb);
	  return (!buffer || real_size < 1) ? 0 : buffer->copy(ptr, real_size);
	}

	size_t web_api::response_callback(char* ptr, size_t size, size_t nmemb, void* response)
	{
		std::string* data(reinterpret_cast<std::string*>(response));

		const size_t real_size(size * nmemb);
		if (!data || real_size < 1) {
			return 0;
		}
		data->append(ptr, real_size);
		return real_size;
	}

	web_api::web_api(const uri& uri)
	:
		web_api(uri.host(), (uri.parameter<std::string>("secure", "false").compare("true") == 0))
	{

	}

	web_api::web_api(const std::string& hostname, bool secure)
	:
		_hostname(hostname),
		_secure(secure),
		_request_code(CURL_LAST),
		_response_code(0)
	{

	}

	web_api::web_api(web_api&& origin)
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

	web_api::~web_api()
	{

	}

	bool web_api::perform(const std::shared_ptr<CURL>& curl, const std::string& path)
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

		_request_code = curl_easy_perform(curl.get());

		if (_request_code != CURLE_OK) {
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

	bool web_api::post(const std::string& path, const mime& content_type, const std::string& content_data)
	{
		return send(path, content_type, content_data, false);
	}

	bool web_api::put(const std::string& path, const mime& content_type, const std::string& content_data)
	{
		return send(path, content_type, content_data, true);
	}

	bool web_api::send(const std::string& path, const mime& content_type, const std::string& content_data, bool put)
	{
		request_buffer buffer{content_data, 0};
		const std::shared_ptr<CURL> curl(curl_easy_init(), curl_easy_cleanup);
		curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 15L);
		curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION, web_api::request_callback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, web_api::response_callback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &_response_body);
		const std::shared_ptr<struct curl_slist> header_slist(curl_slist_append(nullptr, std::string("Content-Type: ").append(content_type.content_type()).data()), curl_slist_free_all);
		if (!header_slist) {
			_request_code = CURLE_HTTP_RETURNED_ERROR;
			return false;
		} else if (put) {
			curl_easy_setopt(curl.get(), CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl.get(), CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(content_data.length()));
			curl_easy_setopt(curl.get(), CURLOPT_READDATA, &buffer);
		} else {
			curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE, static_cast<long>(content_data.length()));
			curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, content_data.data());
		}
		curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, header_slist.get());

		return perform(curl, path);
	}

	bool web_api::get(const std::string& path)
	{
		const std::shared_ptr<CURL> curl(curl_easy_init(), curl_easy_cleanup);
		curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 15L);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, web_api::response_callback);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &_response_body);
		curl_easy_setopt(curl.get(), CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, nullptr);

		return perform(curl, path);
	}

	web_api::request_code web_api::get_error_code() const
	{
		return _request_code;
	}

	std::uint16_t web_api::get_response_code() const
	{
		return _response_code;
	}

	std::string web_api::get_error_string() const
	{
		return (CURLE_OK == _request_code) ? std::string() : std::string(curl_easy_strerror(_request_code));
	}

	std::string::size_type web_api::get_content_length() const
	{
		return _response_body.length();
	}

	const mime& web_api::get_content_type() const
	{
		return _response_type;
	}

	const std::string& web_api::get_content_body() const
	{
		return _response_body;
	}

	bool web_api::is_ok() const
	{
		return CURLE_OK == _request_code && 200 == _response_code;
	}
}
