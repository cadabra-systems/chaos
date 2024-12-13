/**
 @file DeviceToken.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "DeviceToken.hpp"

namespace chaos {
	const bimap<std::string, device_token::push_service> device_token::service_map =
	{
		{
			{"xxx", device_token::push_service::invalid},
			{"apn", device_token::push_service::apn},
			{"fcm", device_token::push_service::fcm},
			{"gcm", device_token::push_service::gcm},
			{"hcm", device_token::push_service::hcm},
			{"rsm", device_token::push_service::rsm},
			{"wns", device_token::push_service::wns}
		},
		device_token::push_service::invalid
	};

	device_token::device_token(const std::string& token, const std::string& ident, device_token::push_service push_service)
	:
		_token(token),
		_ident(ident),
		_push_service(push_service)
	{

	}

	device_token::device_token(const std::string& token, const std::string& ident)
	:
		_token(token),
		_ident(ident),
		_push_service(push_service::invalid)
	{
//		if token starts with 'A@' -> apn, 'F@' -> fcm, 'H@' -> hcm, 'R@' -> rsm
		if (token.length() == 64) {
			_push_service = push_service::apn;
		} else if (token.length() > 64) {
			_push_service = push_service::fcm;
		}
	}

	device_token::device_token(const std::string& cns)
	:
		_push_service(push_service::invalid)
	{
		const std::string::size_type at_offset(cns.find_first_of('@'));
		const std::string::size_type dot_offset(cns.find_last_of('.'));

		if ((at_offset != std::string::npos && dot_offset != std::string::npos) || dot_offset < at_offset) {
			_token = cns.substr(0, at_offset);
			_ident = cns.substr(at_offset + 1, dot_offset - at_offset - 1);
			_push_service = device_token::service_map.at(cns.substr(dot_offset + 1), true);
		}
	}

	device_token::device_token(const device_token& origin)
	:
		_token(origin._token),
		_ident(origin._ident),
		_push_service(origin._push_service)
	{

	}

	device_token::device_token(device_token&& origin)
	:
		_token(std::move(origin._token)),
		_ident(std::move(origin._ident)),
		_push_service(std::move(origin._push_service))
	{

	}

	device_token& device_token::operator=(const device_token& rhs)
	{
		_token = rhs._token;
		_ident = rhs._ident;
		_push_service = rhs._push_service;

		return *this;
	}

	device_token& device_token::operator=(device_token&& rhs)
	{
		_token = std::move(rhs._token);
		_ident = std::move(rhs._ident);
		_push_service = std::move(rhs._push_service);

		return *this;
	}

	bool device_token::operator<(const device_token& rhs) const
	{
		return _token < rhs._token;
	}

	void device_token::set(const std::string& token, device_token::push_service push_service)
	{
		_token = token;
		_push_service = push_service;
	}

	const std::string& device_token::token() const
	{
		return _token;
	}

	const std::string& device_token::ident() const
	{
		return _ident;
	}

	const device_token::push_service& device_token::service() const
	{
		return _push_service;
	}

	std::string device_token::cns() const
	{
		return _token + "@" + _ident + "." + device_token::service_map.at(_push_service, true);
	}

	bool device_token::valid() const
	{
		return push_service::invalid != _push_service;
	}
}
