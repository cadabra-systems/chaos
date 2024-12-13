//
//  IPAddress.cpp
//
//  Created by Daniil A Megrabyan on 26.08.16.
//  Copyright © 2016 Cadabra Systems. All rights reserved.
//

#include "IPAddress.hpp"

#include <algorithm>
#include <arpa/inet.h>

namespace chaos {
	ip_address::family ip_address::version(const std::string& host)
	{
		/// @todo ipv6
		return std::count(host.cbegin(), host.cend(), '.') == 3 && std::all_of(host.begin(), host.end(), [](char c){ return std::isdigit(c) || c == '.'; }) ? ip_address::family::v4 : ip_address::family::unknown;
	}

	void ip_address::addrinfo_deleter::operator()(addrinfo* raw_pointer) const
	{
		freeaddrinfo(raw_pointer);
	}

	ip_address::ip_address()
	:
		ip_address("0.0.0.0", false)
	{
		
	}
	
	ip_address::ip_address(std::string host, bool resolve)
	:
		_info(nullptr)
	{
		set(host, resolve);
	}
	
	ip_address::ip_address(ip_address&& origin)
	:
		_info(origin._info)
	{
		origin._info = nullptr;
	}

	ip_address::ip_address(const ip_address& origin)
	:
		_info(origin._info)
	{

	}

	ip_address& ip_address::operator=(const ip_address& origin)
	{
		_info = origin._info;
		return *this;
	}
	
	ip_address& ip_address::operator=(ip_address&& origin)
	{
		_info = origin._info;
		origin._info = nullptr;
		return *this;
	}

	bool ip_address::operator>(const ip_address& rhs) const
	{
		/// @todo
		return false;
	}

	bool ip_address::operator>=(const ip_address& rhs) const
	{
		/// @todo
		return false;
	}

	bool ip_address::operator<(const ip_address& rhs) const
	{
		/// @todo
		return false;
	}

	bool ip_address::operator<=(const ip_address& rhs) const
	{
		/// @todo
		return false;
	}
	
	bool ip_address::operator==(const ip_address& rhs) const
	{
		/// @todo
		return false;
	}
	
	bool ip_address::operator!=(const ip_address& rhs) const
	{
		return !((*this) == rhs);
	}
	
	ip_address::operator std::string() const
	{
		return make_string();
	}
	
	bool ip_address::set(const std::string& host, bool resolve)
	{
		if (_info != nullptr) {
			_info.reset();
		}
		
		struct addrinfo hint = {resolve ? 0 : AI_NUMERICHOST, PF_UNSPEC, 0, 0, 0, nullptr, nullptr, nullptr};
		
		struct addrinfo* raw;
		if (getaddrinfo(host.c_str(), nullptr, &hint, &raw) != 0) {
			_info = nullptr;
			return false;
		}
		_info.reset(raw, ip_address::addrinfo_deleter());
		
		return true;
	}
	
	std::uint64_t ip_address::make_integer() const
	{
		if (nullptr == _info) {
			return 0;
		} else if (AF_INET == _info->ai_family) {
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(_info->ai_addr);
			return htonl(ipv4->sin_addr.s_addr);
		} else if (AF_INET6 == _info->ai_family) {
//			sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6*>(_info->ai_addr);
			/// @todo
			return 0;
		}
		return 0;
	}

	std::string ip_address::make_string() const
	{
		if (nullptr == _info) {
			return "";
		}
		
		void* addr;
		if (AF_INET == _info->ai_family) {
			sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(_info->ai_addr);
			addr = &(ipv4->sin_addr);
		} else if (AF_INET6 == _info->ai_family) {
			sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6*>(_info->ai_addr);
			addr = &(ipv6->sin6_addr);
		} else {
			return "";
		}
		
		char str[INET6_ADDRSTRLEN];
		if (inet_ntop(_info->ai_family, addr, str, INET6_ADDRSTRLEN) == nullptr) {
			return "";
		}
		
		return std::string(str);
	}
	
	ip_address::family ip_address::get_family() const
	{
		if (nullptr == _info) {
			return family::unknown;
		}
		
		if (AF_INET == _info->ai_family) {
			return family::v4;
		} else if (AF_INET6 == _info->ai_family) {
			return family::v6;
		}
		
		return family::unknown;
	}
}
