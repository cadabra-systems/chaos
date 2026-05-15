/**
 @file Message.cpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Message.hpp"

#include <algorithm>

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

	std::set<std::string> message::make_header_set() const noexcept
	{
		std::set<std::string> retval;
		if (!_pointer) {
			return retval;
		}
		rd_kafka_headers_t* headers;
		if (rd_kafka_message_headers(_pointer, &headers) != RD_KAFKA_RESP_ERR_NO_ERROR) {
			return retval;
		}
		const char* name;
		const void* value_data;
		std::size_t value_size;
		for (std::size_t i = 0; rd_kafka_header_get_all(headers, i, &name, &value_data, &value_size) == RD_KAFKA_RESP_ERR_NO_ERROR; ++i) {
			std::string lowercase_name(name);
			std::transform(lowercase_name.begin(), lowercase_name.end(), lowercase_name.begin(), [](unsigned char c) { return std::tolower(c); });
			retval.emplace(std::move(lowercase_name));
		}
		return retval;
	}

	std::unordered_map<std::string, std::string> message::make_header_map() const noexcept
	{
		std::unordered_map<std::string, std::string> retval;
		if (!_pointer) {
			return retval;
		}
		rd_kafka_headers_t* headers;
		if (rd_kafka_message_headers(_pointer, &headers) != RD_KAFKA_RESP_ERR_NO_ERROR) {
			return retval;
		}
		const char* name;
		const void* value_data;
		std::size_t value_size;
		for (std::size_t i = 0; rd_kafka_header_get_all(headers, i, &name, &value_data, &value_size) == RD_KAFKA_RESP_ERR_NO_ERROR; ++i) {
			retval.insert_or_assign(name, std::string{static_cast<const char*>(value_data), value_size});
		}
		return retval;
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

	std::string_view message::get_header(const std::string& name) const noexcept
	{
		if (!_pointer) {
			return std::string_view{};
		}
		rd_kafka_headers_t* headers;
		if (rd_kafka_message_headers(_pointer, &headers) != RD_KAFKA_RESP_ERR_NO_ERROR) {
			return std::string_view{};
		}
		const void* value;
		std::size_t value_size;
		if (rd_kafka_header_get_last(headers, name.c_str(), &value, &value_size) != RD_KAFKA_RESP_ERR_NO_ERROR) {
			return std::string_view{};
		}
		return std::string_view(static_cast<const char*>(value), value_size);
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

	bool message::has_header(const std::string& name) const noexcept
	{
		if (!_pointer) {
			return false;
		}
		rd_kafka_headers_t* headers;
		if (rd_kafka_message_headers(_pointer, &headers) != RD_KAFKA_RESP_ERR_NO_ERROR) {
			return false;
		}
		const void* value_data;
		std::size_t value_size;
		return rd_kafka_header_get_last(headers, name.c_str(), &value_data, &value_size) == RD_KAFKA_RESP_ERR_NO_ERROR;
	}
} }
