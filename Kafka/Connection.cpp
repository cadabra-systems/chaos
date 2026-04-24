/**
 @file Connection.cpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Connection.hpp"

#include "Log.hpp"

namespace chaos { namespace kafka {
	connection::connection(const std::set<std::pair<std::string, std::uint16_t>>& broker_set, const std::map<std::string, std::string>& configuration_map)
	:
		_broker_set(broker_set),
		_configuration_map(configuration_map),
		_handle(nullptr)
	{

	}

	connection::connection(const std::string& hostname, std::uint16_t port, const std::map<std::string, std::string>& configuration_map)
	:
		connection
		(
			{std::make_pair(hostname, port)},
			configuration_map
		)
	{

	}

	connection::connection(connection&& origin)
	:
		_broker_set(std::move(origin._broker_set)),
		_configuration_map(std::move(origin._configuration_map)),

		_handle(origin._handle)
	{
		origin._handle = nullptr;
	}

	connection::~connection()
	{
		if (_handle) {
			rd_kafka_destroy(_handle);
		}
	}

	bool connection::configure(const std::map<std::string, std::string>& map)
	{
		if (_handle) {
			return false;
		}
		_configuration_map = map;
		return true;
	}

	bool connection::configure(const std::string& key, const std::string& value)
	{
		if (_handle) {
			return false;
		}
		_configuration_map[key] = value;
		return true;
	}

	bool connection::configure(std::nullptr_t)
	{
		if (_handle) {
			return false;
		}
		_configuration_map.clear();
		return true;
	}

	bool connection::connect(const std::string& username, const std::string& password) noexcept
	{
		if (_handle) {
			return true;
		}

		char error_message[512];
		std::string server_line;
		for (const std::pair<std::string, std::uint16_t>& broker : _broker_set) {
			if (!server_line.empty()) {
				server_line += ',';
			}
			server_line += broker.first + ':' + std::to_string(broker.second);
		}
		std::unique_ptr<rd_kafka_conf_t, decltype(&rd_kafka_conf_destroy)> configuration(rd_kafka_conf_new(), rd_kafka_conf_destroy);
		if (rd_kafka_conf_set(configuration.get(), "bootstrap.servers", server_line.c_str(), error_message, sizeof(error_message)) != RD_KAFKA_CONF_OK) {
			chaos::log_register<kafka::log>::error("connection::connect > bootstrap.servers: ", error_message);
			return false;
		}
		if (username.empty()) {
			_configuration_map.erase("sasl.username");
			_configuration_map.erase("sasl.password");
			_configuration_map.erase("security.protocol");
			_configuration_map.erase("sasl.mechanisms");
		} else {
			_configuration_map["sasl.username"] = username;
			_configuration_map["sasl.password"] = password;
			_configuration_map["security.protocol"] = "SASL_PLAINTEXT";
			_configuration_map["sasl.mechanisms"] = "SCRAM-SHA-256";
		}
		for (const std::map<std::string, std::string>::value_type& config : _configuration_map) {
			if (rd_kafka_conf_set(configuration.get(), config.first.c_str(), config.second.c_str(), error_message, sizeof(error_message)) != RD_KAFKA_CONF_OK) {
				chaos::log_register<kafka::log>::warning("connection::connect > ", config.first, ": ", error_message);
			}
		}
		const rd_kafka_type_t type(_configuration_map.find("group.id") == _configuration_map.cend() ? RD_KAFKA_PRODUCER : RD_KAFKA_CONSUMER);
		/// @note release() — не утечка: rd_kafka_new() всегда освобождает conf сам, независимо от результата
		if (!(_handle = rd_kafka_new(type, configuration.release(), error_message, sizeof(error_message)))) {
			chaos::log_register<kafka::log>::error("connection::connect > error: ", error_message);
			_configuration_map.erase("sasl.username");
			_configuration_map.erase("sasl.password");
			_configuration_map.erase("security.protocol");
			_configuration_map.erase("sasl.mechanisms");
			return false;
		}
		return true;
	}

	bool connection::disconnect() noexcept
	{
		if (!_handle) {
			return false;
		}

		rd_kafka_destroy(_handle);
		_handle = nullptr;
		return true;
	}

	bool connection::reconnect() noexcept
	{
		if (!disconnect()) {
			return false;
		}
		const std::map<std::string, std::string>::const_iterator username(_configuration_map.find("sasl.username"));
		const std::map<std::string, std::string>::const_iterator password(_configuration_map.find("sasl.password"));
		return (username == _configuration_map.cend() || password == _configuration_map.cend()) ? connect() : connect(username->second, password->second);
	}

	bool connection::create_topic(const std::string& name, int partition_count, int replication_factor, int timeout) noexcept
	{
		if (!_handle) {
			return false;
		}

		char error_message[512];
		std::unique_ptr<rd_kafka_NewTopic_t, decltype(&rd_kafka_NewTopic_destroy)> new_topic(
			rd_kafka_NewTopic_new(name.c_str(), partition_count, replication_factor, error_message, sizeof(error_message)),
			rd_kafka_NewTopic_destroy
		);
		if (!new_topic) {
			chaos::log_register<kafka::log>::error("connection::create_topic > ", error_message);
			return false;
		}

		std::unique_ptr<rd_kafka_queue_t, decltype(&rd_kafka_queue_destroy)> queue(rd_kafka_queue_new(_handle), rd_kafka_queue_destroy);
		rd_kafka_NewTopic_t* raw_topic(new_topic.get());
		rd_kafka_CreateTopics(_handle, &raw_topic, 1, nullptr, queue.get());
		new_topic.reset();

		std::unique_ptr<rd_kafka_event_t, decltype(&rd_kafka_event_destroy)> event(rd_kafka_queue_poll(queue.get(), timeout), rd_kafka_event_destroy);
		if (!event) {
			chaos::log_register<kafka::log>::error("connection::create_topic > no result received (timeout=", timeout, ")");
			return false;
		}

		std::size_t topic_count(0);
		const rd_kafka_topic_result_t** topic_result_list(rd_kafka_CreateTopics_result_topics(rd_kafka_event_CreateTopics_result(event.get()), &topic_count));
		for (std::size_t i = 0; i < topic_count; ++i) {
			const rd_kafka_resp_err_t error(rd_kafka_topic_result_error(topic_result_list[i]));
			if (error != RD_KAFKA_RESP_ERR_NO_ERROR && error != RD_KAFKA_RESP_ERR_TOPIC_ALREADY_EXISTS) {
				chaos::log_register<kafka::log>::error("connection::create_topic > ", rd_kafka_err2str(error));
				return false;
			}
		}
		return true;
	}

	bool connection::delete_topic(const std::string& name, int timeout) noexcept
	{
		if (!_handle) {
			return false;
		}

		std::unique_ptr<rd_kafka_DeleteTopic_t, decltype(&rd_kafka_DeleteTopic_destroy)> topic_handle(
			rd_kafka_DeleteTopic_new(name.c_str()),
			rd_kafka_DeleteTopic_destroy
		);

		std::unique_ptr<rd_kafka_queue_t, decltype(&rd_kafka_queue_destroy)> queue(rd_kafka_queue_new(_handle), rd_kafka_queue_destroy);
		rd_kafka_DeleteTopic_t* raw_topic(topic_handle.get());
		rd_kafka_DeleteTopics(_handle, &raw_topic, 1, nullptr, queue.get());
		topic_handle.reset();

		std::unique_ptr<rd_kafka_event_t, decltype(&rd_kafka_event_destroy)> event(rd_kafka_queue_poll(queue.get(), timeout), rd_kafka_event_destroy);
		if (!event) {
			chaos::log_register<kafka::log>::error("connection::delete_topic > no result received (timeout=", timeout, ")");
			return false;
		}

		std::size_t topic_count(0);
		const rd_kafka_topic_result_t** topic_result_list(rd_kafka_DeleteTopics_result_topics(rd_kafka_event_DeleteTopics_result(event.get()), &topic_count));
		for (std::size_t i = 0; i < topic_count; ++i) {
			const rd_kafka_resp_err_t error(rd_kafka_topic_result_error(topic_result_list[i]));
			if (error != RD_KAFKA_RESP_ERR_NO_ERROR && error != RD_KAFKA_RESP_ERR_UNKNOWN_TOPIC_OR_PART) {
				chaos::log_register<kafka::log>::error("connection::delete_topic > ", rd_kafka_err2str(error));
				return false;
			}
		}
		return true;
	}

	bool connection::alive() noexcept
	{
		if (!_handle) {
			return false;
		}
		const rd_kafka_metadata_t* metadata(nullptr);
		const rd_kafka_resp_err_t error(rd_kafka_metadata(_handle, 0, nullptr, &metadata, 3000));
		if (metadata) {
			rd_kafka_metadata_destroy(metadata);
		}
		return (error == RD_KAFKA_RESP_ERR_NO_ERROR) || reconnect();
	}

	const std::map<std::string, std::string>& connection::get_configuration_map() const noexcept
	{
		return _configuration_map;
	}

	bool connection::is_active() const noexcept
	{
		return _handle != nullptr;
	}
} }
