/**
 @file Message.cpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Message.hpp"

namespace chaos { namespace kafka {
	message::message(rd_kafka_message_t* raw) noexcept
	:
		_pointer(raw)
	{

	}

	message::message(message&& origin) noexcept
	:
		_pointer(origin._pointer)
	{
		origin._pointer = nullptr;
	}

	message::~message()
	{
		if (_pointer) {
			rd_kafka_message_destroy(_pointer);
		}
	}

	message& message::operator=(message&& origin) noexcept
	{
		if (_pointer) {
			rd_kafka_message_destroy(_pointer);
		}
		_pointer = origin._pointer;
		origin._pointer = nullptr;
		return *this;
	}

	std::string_view message::get_topic() const noexcept
	{
		return (_pointer && _pointer->rkt) ? std::string_view(rd_kafka_topic_name(_pointer->rkt)) : std::string_view{};
	}

	std::string_view message::get_key() const noexcept
	{
		return (_pointer && _pointer->key) ? std::string_view(static_cast<const char*>(_pointer->key), _pointer->key_len) : std::string_view{};
	}

	std::string_view message::get_payload() const noexcept
	{
		return (_pointer && _pointer->payload) ? std::string_view(static_cast<const char*>(_pointer->payload), _pointer->len) : std::string_view{};
	}

	std::int32_t message::get_partition() const noexcept
	{
		return _pointer ? _pointer->partition : RD_KAFKA_PARTITION_UA;
	}

	std::int64_t message::get_offset() const noexcept
	{
		return _pointer ? _pointer->offset : RD_KAFKA_OFFSET_INVALID;
	}

	std::string_view message::get_error_string() const noexcept
	{
		return (_pointer && _pointer->err != RD_KAFKA_RESP_ERR_NO_ERROR) ? std::string_view(rd_kafka_err2str(_pointer->err)) : std::string_view{};
	}

	bool message::is_error() const noexcept
	{
		return _pointer && _pointer->err != RD_KAFKA_RESP_ERR_NO_ERROR;
	}

	bool message::is_empty() const noexcept
	{
		return !_pointer;
	}
} }
