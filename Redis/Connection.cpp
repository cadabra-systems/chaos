/**
 @file Connection.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "Connection.hpp"

#include "Reply.hpp"
#include "CommandLine.hpp"
#include "Log.hpp"

#include <errno.h>

namespace chaos { namespace redis {
	connection::error::error(const connection& connection)
	:
		_connection(connection)
	{

	}
	
	std::string connection::error::get_message() const
	{
		const incident i(get_incident());
		if (incident::ok == i) {
			return "";
		}
		
		std::string retval(_connection._handle->errstr);
		if (i == incident::io) {
			/// @bug strerror_r?
			retval.append("(" + std::string(strerror(errno)) + ")");
		}
		
		return retval;
	}
	
	connection::error::incident connection::error::get_incident() const
	{
		return (nullptr == _connection._handle) ? incident::ok : static_cast<connection::error::incident>(_connection._handle->err);
	}
	
	connection::connection(const std::string& host, const std::uint16_t port)
	:
		_host(host),		
		_port(port),
		_handle(nullptr),
		_error(*this)
	{
		
	}
	
	connection::~connection()
	{
		disconnect();
	}
	
	bool connection::connect()
	{
		if (is_connected()) {
			return true;
		}

		_handle = redisConnectWithTimeout(_host.c_str(), _port, {1, 500000});
		
		if (nullptr == _handle || _handle->err) {
			return false;
		}
		
		// client setname
		
		return true;
	}
	
	bool connection::reconnect()
	{
		if (nullptr == _handle) {
			return false;
		}
		
		if (redisReconnect(_handle) != REDIS_OK) {
			return false;
		}
		
		return true;
	}
	
	bool connection::disconnect()
	{
		redisFree(_handle);
		_handle = nullptr;
		
		return true;
	}
	
	bool connection::send(command& command) const
	{
		if (!is_connected()) {
			return false;
		}
		
		command_line line(command.make_line());
		if (line.is_empty()) {
			return false;
		}

		const auto start = std::chrono::system_clock::now();
		chaos::log_register<redis::log>::debug("Command(", std::addressof(command), ") * Command is executing: ", static_cast<std::string>(line));
		redisReply* reply_handle = (redisReply*)redisCommandArgv(_handle, line.get_count(), line.get_value_data(), line.get_length_data());
		if (nullptr == reply_handle) {
			command.reset_reply(std::make_unique<reply>(nullptr));
			chaos::log_register<redis::log>::error("Command(", std::addressof(command), ") > connection error(", static_cast<std::uint8_t>(get_error().get_incident()), "): " + get_error().get_message());
			return false;
		}
		std::unique_ptr<reply> r(std::unique_ptr<reply>(new reply(reply_handle)));
		bool retval(true);
		if (r->is_faulty()) {
			retval = false;
		}
		command.reset_reply(std::move(r));
		
		if (retval) {
			chaos::log_register<redis::log>::notice("Command(", std::addressof(command), ") > Command has been successfully executed within ", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count(), " ms.");
		} else {
			chaos::log_register<redis::log>::error("Command(", std::addressof(command), ") > Command has been completed with errors");
		}
		
		return retval;
	}
	
	bool connection::send(std::initializer_list<std::reference_wrapper<command>> list, bool) const
	{
		if (!is_connected()) {
			return false;
		}

		const auto start = std::chrono::system_clock::now();
		for (const command& command: list) {
			command_line line(command.make_line());
			chaos::log_register<redis::log>::debug("Command(", std::addressof(command), ") * Command is executing: ", static_cast<std::string>(line));
			redisAppendCommandArgv(_handle, line.get_count(), line.get_value_data(), line.get_length_data());
		}

		bool retval(true);
		for (command& command: list) {
			redisReply* reply_handle;
			if (redisGetReply(_handle, (void**)(&reply_handle)) != REDIS_OK) {
				command.reset_reply(std::make_unique<reply>(nullptr));
				retval = false;
				chaos::log_register<redis::log>::error("Command(", std::addressof(command), ") > connection error(", static_cast<std::uint8_t>(get_error().get_incident()), "): " + get_error().get_message());
			} else {
				std::unique_ptr<reply> r(std::unique_ptr<reply>(new reply(reply_handle)));
				if (r->is_faulty()) {
					retval = false;
					chaos::log_register<redis::log>::error("Command(", std::addressof(command), ") > Command has been completed with errors");
				} else {
					chaos::log_register<redis::log>::notice("Command(", std::addressof(command), ") > Command has been successfully executed within ", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count(), " ms.");
				}
				command.reset_reply(std::move(r));
			}
		}		
		
		return retval;
	}
	
	const connection::error& connection::get_error() const
	{
		return _error;
	}
	
	bool connection::is_connected() const
	{
		return (nullptr != _handle && !_handle->err);
	}
} }

