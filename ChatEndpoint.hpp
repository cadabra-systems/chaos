#/**
@file DeviceToken.hpp
@date 2014-01-01
@copyright Cadabra Systems
@author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ChatEndoint_hpp
#define Chaos_ChatEndoint_hpp

#include "String.hpp"
#include "BiMap.hpp"

#include <string>
#include <cstdint>

namespace chaos {
	class chat_endpoint
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class message_service : std::uint8_t
		{
			invalid = '0',
			cap = 'C',
			whatsapp = 'W',
			viber = 'V',
			telegram = 'T'
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Statics*/
	/** @{ */
	public:
		static const bimap<std::string, chat_endpoint::message_service> service_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		chat_endpoint(const std::string& chat_token, const std::string& bot_token, chat_endpoint::message_service service);
		chat_endpoint(const std::string& chat_token, const std::string& bot_token);
		chat_endpoint(const std::string& cns);
		chat_endpoint(const chat_endpoint& origin);
		chat_endpoint(chat_endpoint&& origin);

		~chat_endpoint() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _bot_token;
		std::string _chat_token;
		message_service _message_service;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		chat_endpoint& operator=(const chat_endpoint& rhs);
		chat_endpoint& operator=(chat_endpoint&& rhs);
		bool operator<(const chat_endpoint& rhs) const;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void parse(const std::string& cns);
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set(const std::string& bot_token, const std::string& chat_token, chat_endpoint::message_service service);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& chat_token() const;
		const std::string& bot_token() const;
		const chat_endpoint::message_service& service() const;
		std::string cns() const;
	/** @} */

	/** @name States*/
	/** @{ */
	public:
		bool valid() const;
	/** @} */
	};
}

#endif
