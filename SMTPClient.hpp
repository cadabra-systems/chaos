/**
 @file SMTPClient.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_SMTPClient_hpp
#define Chaos_SMTPClient_hpp

#include "URI.hpp"
#include "EmailMessage.hpp"

#include <string>
#include <memory>

namespace chaos {
	class smtp_client
	{
	/** @name Alias */
	/** @{ */
	public:
		enum class security : std::uint8_t
		{
			none = 0,
			ssl = 1,
			tls = 2,
			start_tls = 3
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static std::shared_ptr<smtp_client> instance(const chaos::uri& uri);

	private:
		static security convert_security(const std::string& value);
		static size_t payload_callback(char* ptr, size_t size, size_t nmemb, void* userp);
	/** @} */

	/** @name Alias */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		smtp_client(const chaos::uri& uri);
		smtp_client(const std::string& hostname, unsigned short port, security security, const std::string& username, const std::string& password);
		smtp_client(smtp_client&& origin);
		smtp_client(smtp_client const&) = delete;
		virtual ~smtp_client();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const std::string _hostname;
		const unsigned short _port;
		const security _security;
		const std::string _username;
		const std::string _password;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
		bool send(const email_message& message);
	/** @} */

	/** @name Hooks */
	/** @{ */
	protected:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		smtp_client& operator=(smtp_client&& right) = delete;
		smtp_client& operator=(smtp_client&) = delete;
		smtp_client& operator=(smtp_client const&) = delete;
		smtp_client& operator=(smtp_client const&&) = delete;
	/** @} */
	};
}

#endif
