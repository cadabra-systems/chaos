/**
 @file HTTPClient.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_HTTPClient_hpp
#define Chaos_HTTPClient_hpp

#include "OwnerPointer.hpp"
#include "URI.hpp"
#include "MIME.hpp"
#include "Flex.hpp"

#include <curl/curl.h>

#include <map>
#include <string>

namespace chaos {
	class http_client
	{
	/** @name Statics */
	/** @{ */
	private:
		static size_t debug_callback(CURL* handle, curl_infotype type, char* data, size_t size, void* client);
		static size_t request_callback(char* ptr, size_t size, size_t nmemb, void* request);
		static size_t response_callback(char* ptr, size_t size, size_t nmemb, void* response);
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using header_map = std::map<std::string, std::string>;
		using request_code = CURLcode;
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
		enum class http_code : std::uint16_t
		{
			none = 0,
			ok = 200,
			not_found = 404
		};

		enum class send_mode : char
		{
			post = 'O',
			put = 'U',
			patch = 'A'
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
		http_client(const uri& uri);
		http_client(const std::string& hostname, bool secure);
		http_client(http_client&& origin);
		http_client(http_client const&) = delete;
		virtual ~http_client();
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		const std::string _hostname;
		const bool _secure;

	private:
		request_code _request_code;
		header_map _header_map;

		std::uint16_t _response_code;
		std::string _response_body;
		mime _response_type;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	private:
		bool perform(const owner_ptr<CURL>& curl, const std::string& path, const header_map& header_map = {});

	public:
		bool send(const std::string& path, const mime& content_type, const std::string& content_data, send_mode mode = send_mode::post);
		bool send(const std::string& path, const flex& body, send_mode mode = send_mode::post);
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

	/** @name Setters */
	/** @{ */
	public:
		/**
		 * @brief Digest
		 * @param id
		 * @param password
		 */
		void authorize(const std::string& id, const std::string& password);
		/**
		 * @brief Bearer
		 * @param token
		 */
		void authorize(const std::string& token);
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		http_client& operator=(http_client&& right) = delete;
		http_client& operator=(http_client&) = delete;
		http_client& operator=(http_client const&) = delete;
		http_client& operator=(http_client const&&) = delete;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_ok() const;
	/** @} */
	};
}

#endif
