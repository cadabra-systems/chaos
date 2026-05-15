/**
 @file Producer.cpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Producer.hpp"
#include "Log.hpp"

namespace chaos { namespace kafka {
	producer::producer(const std::set<std::pair<std::string, std::uint16_t>>& broker_set, const std::map<std::string, std::string>& configuration_map)
	:
		connection(broker_set, configuration_map)
	{

	}

	producer::producer(const std::string& hostname, std::uint16_t port, const std::map<std::string, std::string>& configuration_map)
	:
		connection(hostname, port, configuration_map)
	{

	}

	bool producer::connect(const std::string& username, const std::string& password) noexcept
	{
		return _handle || connection::connect(username, password);
	}

	bool producer::disconnect() noexcept
	{
		if (!_handle) {
			return false;
		}
		if (rd_kafka_flush(_handle, 5000) == RD_KAFKA_RESP_ERR__TIMED_OUT) {
			chaos::log_register<kafka::log>::warning("producer::disconnect > flush timed out, some messages may be lost");
		}
		return connection::disconnect();
	}

	bool producer::push(const std::string& topic, std::string_view payload, std::string_view key, const std::map<std::string, std::string>& header_map) noexcept
	{
		if (!_handle) {
			return false;
		}

		rd_kafka_headers_t* headers(nullptr);
		if (!header_map.empty()) {
			headers = rd_kafka_headers_new(header_map.size());
			for (const auto& [name, value] : header_map) {
				rd_kafka_header_add(headers, name.c_str(), name.size(), value.c_str(), value.size());
			}
		}

		rd_kafka_resp_err_t error;
		if (key.empty() && !headers) {
			error = rd_kafka_producev
			(
				_handle,
				RD_KAFKA_V_TOPIC(topic.c_str()),
				RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
				RD_KAFKA_V_VALUE(const_cast<char*>(payload.data()), payload.size()),
				RD_KAFKA_V_END
			);
		} else if (key.empty()) {
			error = rd_kafka_producev
			(
				_handle,
				RD_KAFKA_V_TOPIC(topic.c_str()),
				RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
				RD_KAFKA_V_VALUE(const_cast<char*>(payload.data()), payload.size()),
				RD_KAFKA_V_HEADERS(headers),
				RD_KAFKA_V_END
			);
		} else if (!headers) {
			error = rd_kafka_producev
			(
				_handle,
				RD_KAFKA_V_TOPIC(topic.c_str()),
				RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
				RD_KAFKA_V_VALUE(const_cast<char*>(payload.data()), payload.size()),
				RD_KAFKA_V_KEY(const_cast<char*>(key.data()), key.size()),
				RD_KAFKA_V_END
			);
		} else {
			error = rd_kafka_producev
			(
				_handle,
				RD_KAFKA_V_TOPIC(topic.c_str()),
				RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
				RD_KAFKA_V_VALUE(const_cast<char*>(payload.data()), payload.size()),
				RD_KAFKA_V_KEY(const_cast<char*>(key.data()), key.size()),
				RD_KAFKA_V_HEADERS(headers),
				RD_KAFKA_V_END
			);
		}

		if (error != RD_KAFKA_RESP_ERR_NO_ERROR) {
			if (headers) {
				rd_kafka_headers_destroy(headers);
			}
			chaos::log_register<kafka::log>::error("producer::produce > ", rd_kafka_err2str(error));
			return false;
		}
		rd_kafka_poll(_handle, 0);
		return true;
	}

	bool producer::flush(int timeout) noexcept
	{
		return _handle && (rd_kafka_flush(_handle, timeout) == RD_KAFKA_RESP_ERR_NO_ERROR);
	}
} }
