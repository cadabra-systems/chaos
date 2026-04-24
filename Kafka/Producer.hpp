/**
 @file Producer.hpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Kafka_Producer_hpp
#define Chaos_Kafka_Producer_hpp

#include "Connection.hpp"

#include <string>
#include <string_view>

namespace chaos { namespace kafka {
	class producer : public connection
	{
	/** @name Constructors */
	/** @{ */
	public:
		producer(const std::set<std::pair<std::string, std::uint16_t>>& broker_set, const std::map<std::string, std::string>& configuration_map = {});
		producer(const std::string& hostname, std::uint16_t port, const std::map<std::string, std::string>& configuration_map = {});
		producer(producer&&) = default;
		producer(const producer&) = delete;
		virtual ~producer() override = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		producer& operator=(producer&&) = delete;
		producer& operator=(const producer&) = delete;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual bool connect(const std::string& username, const std::string& password) noexcept override;
		virtual bool disconnect() noexcept override;

		bool produce(const std::string& topic, std::string_view payload, std::string_view key = {}) noexcept;
		bool flush(int timeout = 1000) noexcept;
	/** @} */
	};
} }

#endif
