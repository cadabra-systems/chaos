#/**
@file DeviceToken.hpp
@date 2014-01-01
@copyright Cadabra Systems
@author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_DeviceToken_hpp
#define Chaos_DeviceToken_hpp

#include "String.hpp"
#include "BiMap.hpp"

#include <string>
#include <cstdint>

namespace chaos {
	class device_token
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class push_service : std::uint8_t
		{
			invalid = '0',
			apn = 'A', /// < Apple Push Notification = bc93fc1970d320c60ab5531de407cc7913d4f19b0ae34d14a36b6e1aead6a4e6
			fcm = 'F', /// < Firebase Cloud Messaging = cvGaZrvUmCk:APA91bHLWn9kqD-ujcA9CdAl2SwHt9naDd8g4zlNfe0WLVXDp3vppHUfHJxDYOfXPDjlelQbfmM8jjpQmb58_21gcGZqzmfoe3WXKyrETh78T88h_F1GOEW1FOiU-QthM8yT4qg1wCt3
			gcm = 'G', /// < Google Cloud Messaging
			hcm = 'W', /// < Huawei Cloud Messaging
			rsm = 'R', /// < RuStore Messaging
			wns = 'W' /// < Windows Notification Service
		};
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Statics*/
	/** @{ */
	public:
		static const bimap<std::string, device_token::push_service> service_map;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		device_token(const std::string& token, const std::string& ident, device_token::push_service service);
		device_token(const std::string& token, const std::string& ident);
		device_token(const std::string& cns);
		device_token(const device_token& origin);
		device_token(device_token&& origin);

		~device_token() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		std::string _token;
		std::string _ident;
		push_service _push_service;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		device_token& operator=(const device_token& rhs);
		device_token& operator=(device_token&& rhs);
		bool operator<(const device_token& rhs) const;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void set(const std::string& token, device_token::push_service service);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& token() const;
		const std::string& ident() const;
		const device_token::push_service& service() const;
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
