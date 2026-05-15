/**
 @file Publisher.hpp
 @date 2026-04-28
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_RabbitMQ_Publisher_hpp
#define Chaos_RabbitMQ_Publisher_hpp

#include "Connection.hpp"

#include <map>
#include <string>
#include <string_view>

namespace chaos { namespace rabbitmq {
	class publisher : public connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		publisher(const std::string& hostname, std::uint16_t port, const std::string& vhost = "/");
		publisher(publisher&&);
		publisher(const publisher&) = delete;
		virtual ~publisher() override;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		publisher& operator=(publisher&&) = delete;
		publisher& operator=(const publisher&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool connect(const std::string& username, const std::string& password) noexcept override;
		virtual bool disconnect() noexcept override;

		bool push(const std::string& exchange_name, const std::string& routing_key, std::string_view payload, std::string_view content_type = "application/octet-stream", const std::map<std::string, std::string>& header_map = {}) noexcept;
	/** @} */
	};
} }

#endif
