/**
 @file IPAddress.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_IPAddress_hpp
#define Chaos_IPAddress_hpp

#include <string>
#include <cstdint>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory>

namespace chaos {
	class ip_address
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class family : std::uint8_t
		{
			unknown = 0,
			v4 = 4,
			v6 = 6
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static ip_address::family version(const std::string& host);
		static const std::size_t max_string_length = INET6_ADDRSTRLEN;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		struct addrinfo_deleter
		{
			void operator()(addrinfo* pointer) const;
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ip_address();
		ip_address(ip_address&& origin);
		ip_address(std::string host, bool resolve = false);
		ip_address(const ip_address& origin);
		~ip_address() = default;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		ip_address& operator=(const ip_address& origin);
		ip_address& operator=(ip_address&& origin);
		bool operator>(const ip_address& rhs) const;
		bool operator>=(const ip_address& rhs) const;
		bool operator<(const ip_address& rhs) const;
		bool operator<=(const ip_address& rhs) const;
		bool operator==(const ip_address& rhs) const;
		bool operator!=(const ip_address& rhs) const;
		
		operator std::string() const;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::shared_ptr<addrinfo> _info;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		ip_address::family get_family() const;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
		bool set(const std::string& host, bool resolve = false);
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
		std::uint64_t make_integer() const;
		std::string make_string() const;
	/** @} */
	};
}

#endif
