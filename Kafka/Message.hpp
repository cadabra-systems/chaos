/**
 @file Message.hpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Kafka_Message_hpp
#define Chaos_Kafka_Message_hpp

#include <librdkafka/rdkafka.h>

#include <string_view>
#include <cstdint>

namespace chaos { namespace kafka {
	class message
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit message(rd_kafka_message_t* raw) noexcept;
		message(message&&) noexcept;
		message(const message&) = delete;
		~message();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		rd_kafka_message_t* _pointer;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		message& operator=(message&&) noexcept;
		message& operator=(const message&) = delete;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		std::string_view get_topic() const noexcept;
		std::string_view get_key() const noexcept;
		std::string_view get_payload() const noexcept;
		std::int32_t get_partition() const noexcept;
		std::int64_t get_offset() const noexcept;
		std::string_view get_error_string() const noexcept;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_error() const noexcept;
		bool is_empty() const noexcept;
	/** @} */
	};
} }

#endif
