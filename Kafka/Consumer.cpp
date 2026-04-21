/**
 @file Consumer.cpp
 @date 2026-04-17
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Consumer.hpp"
#include "Log.hpp"

#include <unistd.h>
#include <fcntl.h>

namespace chaos { namespace kafka {
	consumer::consumer(const std::set<std::pair<std::string, std::uint16_t>>& broker_set, const std::map<std::string, std::string>& configuration_map)
	:
		connection(broker_set, configuration_map),

		_queue(nullptr),
		_pipe{-1, -1}
	{

	}

	consumer::consumer(const std::string& hostname, std::uint16_t port, const std::map<std::string, std::string>& configuration_map)
	:
		connection(hostname, port, configuration_map),

		_queue(nullptr),
		_pipe{-1, -1}
	{

	}

	consumer::consumer(consumer&& origin)
	:
		connection(std::move(origin)),

		_queue(origin._queue),
		_pipe{origin._pipe[0], origin._pipe[1]}
	{
		origin._queue = nullptr;
		origin._pipe[0] = -1;
		origin._pipe[1] = -1;
	}

	consumer::~consumer()
	{
		if (_queue) {
			rd_kafka_queue_destroy(_queue);
		}
		if (_handle) {
			rd_kafka_consumer_close(_handle);
		}
		if (_pipe[0] > -1) {
			close(_pipe[0]);
		}
		if (_pipe[1] > -1) {
			close(_pipe[1]);
		}
	}

	bool consumer::connect() noexcept
	{
		std::map<std::string, std::string>::const_iterator g(get_configuration_map().find("group.id"));
		return get_configuration_map().cend() == g ? false : consumer::connect(g->second);
	}

	bool consumer::connect(const std::string& group_id) noexcept
	{
		if (_handle) {
			return true;
		} else if (!configure("group.id", group_id)) {
			return false;
		} else if (!connection::connect(RD_KAFKA_CONSUMER)) {
			return false;
		}

		rd_kafka_poll_set_consumer(_handle);
		if (pipe(_pipe) != 0) {
			chaos::log_register<kafka::log>::error("consumer::connect > pipe failed");
			rd_kafka_consumer_close(_handle);
			rd_kafka_destroy(_handle);
			_handle = nullptr;
			return false;
		}
		fcntl(_pipe[0], F_SETFL, O_NONBLOCK);
		_queue = rd_kafka_queue_get_consumer(_handle);
		rd_kafka_queue_io_event_enable(_queue, _pipe[1], "1", 1);
		return true;
	}

	bool consumer::disconnect() noexcept
	{
		if (!_handle) {
			return false;
		} else if (_queue) {
			rd_kafka_queue_destroy(_queue);
			_queue = nullptr;
		}
		rd_kafka_consumer_close(_handle);
		rd_kafka_destroy(_handle);
		_handle = nullptr;
		if (_pipe[0] > -1) {
			close(_pipe[0]);
			_pipe[0] = -1;
		}
		if (_pipe[1] > -1) {
			close(_pipe[1]);
			_pipe[1] = -1;
		}
		return true;
	}

	bool consumer::subscribe(const std::list<std::string>& topic_name_list) noexcept
	{
		if (!_handle || topic_name_list.empty()) {
			return false;
		}
		rd_kafka_topic_partition_list_t* topic_partition_list(rd_kafka_topic_partition_list_new(static_cast<int>(topic_name_list.size())));
		for (const std::string& topic : topic_name_list) {
			rd_kafka_topic_partition_list_add(topic_partition_list, topic.c_str(), RD_KAFKA_PARTITION_UA);
		}
		const rd_kafka_resp_err_t error(rd_kafka_subscribe(_handle, topic_partition_list));
		rd_kafka_topic_partition_list_destroy(topic_partition_list);
		if (error != RD_KAFKA_RESP_ERR_NO_ERROR) {
			chaos::log_register<kafka::log>::error("consumer::subscribe > ", rd_kafka_err2str(error));
			return false;
		}
		return true;
	}

	bool consumer::unsubscribe() noexcept
	{
		return _handle && (rd_kafka_unsubscribe(_handle) == RD_KAFKA_RESP_ERR_NO_ERROR);
	}

	message consumer::poll(int timeout) noexcept
	{
		return message(_handle ? rd_kafka_consumer_poll(_handle, timeout) : nullptr);
	}

	bool consumer::commit() noexcept
	{
		return _handle && (rd_kafka_commit(_handle, nullptr, 0) == RD_KAFKA_RESP_ERR_NO_ERROR);
	}

	const int& consumer::get_file_descriptor() const noexcept
	{
		return _pipe[0];
	}
} }
