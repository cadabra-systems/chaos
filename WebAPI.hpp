/**
 @file WebAPI.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_WebAPI_hpp
#define Chaos_WebAPI_hpp

#include "URI.hpp"
#include "MIME.hpp"

#include <curl/curl.h>

#include <sstream>
#include <string>
#include <memory>

namespace chaos {
	class web_api
	{
	/** @name Statics */
	/** @{ */
	private:
		static size_t request_callback(char* ptr, size_t size, size_t nmemb, void* request);
		static size_t response_callback(char* ptr, size_t size, size_t nmemb, void* response);
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using request_code = CURLcode;
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
		enum class http_code : std::uint16_t {
			none = 0,
			ok = 200,
			not_found = 404
		};

		struct request_buffer
		{
			const std::string& data;
			std::size_t position;

			std::size_t copy(char* destination, std::size_t length);
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		web_api(const uri& uri);
		web_api(const std::string& hostname, bool secure);
		web_api(web_api&& origin);
		web_api(web_api const&) = delete;
		virtual ~web_api();
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		const std::string _hostname;
		const bool _secure;

	private:
		request_code _request_code;
		std::uint16_t _response_code;
		std::string _response_body;
		mime _response_type;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	private:
		bool perform(const std::shared_ptr<CURL>& curl, const std::string& path);

	public:
		bool post(const std::string& path, const mime& content_type, const std::string& content_data);
		bool put(const std::string& path, const mime& content_type, const std::string& content_data);
		bool send(const std::string& path, const mime& content_type, const std::string& content_data, bool put = false);
		bool get(const std::string& path);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		request_code get_error_code() const;
		std::uint16_t get_response_code() const;
		std::string get_error_string() const;
		std::string::size_type get_content_length() const;
		const mime& get_content_type() const;
		const std::string& get_content_body() const;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		web_api& operator=(web_api&& right) = delete;
		web_api& operator=(web_api&) = delete;
		web_api& operator=(web_api const&) = delete;
		web_api& operator=(web_api const&&) = delete;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_ok() const;
	/** @} */
	};
}

#endif
