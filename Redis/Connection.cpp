/**
 @file Connection.cpp
 @date 2017-11-07
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#include "Connection.hpp"

#include "Log.hpp"
#include "Command/AuthenticateCommand.hpp"
#include "Command/SetClientNameCommand.hpp"
#include "Command/PingCommand.hpp"

namespace chaos { namespace redis {
	connection::connection(const std::string& host, std::uint16_t port, const std::string& name)
	:
		_host(host),
		_port(port),
		_name(name),
		_context(nullptr)
	{

	}

	connection::connection(connection&& origin)
	:
		_host(std::move(origin._host)),
		_port(std::move(origin._port)),
		_name(std::move(origin._name)),
		_context(std::move(origin._context))
	{
		origin._context = nullptr;
	}

	connection::~connection()
	{
		redisFree(_context);
	}

	bool connection::connect(const std::string& username, const std::string& password)
	{
		if (_context) {
			return true;
		}
		redisContext* context(redisConnectWithTimeout(_host.data(), _port, {1, 500000}));
		if (!context) {
			redisFree(context);
			return false;
		}
		do {
			if (
				!_name.empty()
				&&
				std::make_shared<set_client_name_command>(_name)->execute(context) != procedure::state::success
			) {
				chaos::log_register<redis::log>::error("connection(", this, ")::connect > set_client_name error ", _context->err, ": ", _context->errstr);
				break;
			} else if (
						!password.empty()
						&&
						std::make_shared<authenticate_command>(username, password)->execute(context) != procedure::state::success
			) {
				chaos::log_register<redis::log>::error("connection(", this, ")::connect > authenticate error ", _context->err, ": ", _context->errstr);
				break;
			}
			std::swap(_context, context);
			redisFree(context);
			return true;
		} while (false);
		redisFree(context);
		return false;
	}

	bool connection::reconnect()
	{
		return (_context && redisReconnect(_context) == REDIS_OK);
	}

	bool connection::disconnect()
	{
		if (!_context) {
			return false;
		}
		redisFree(_context);
		_context = nullptr;
		return true;
	}

	bool connection::is_active() const
	{
		return _context;
	}

	bool connection::is_faulty() const
	{
		return _context && _context->err;
	}

	sync_connection::sync_connection(const std::string& host, std::uint16_t port, const std::string& name)
	:
		connection(host, port, name)
	{

	}

	sync_connection::sync_connection(sync_connection&& origin)
	:
		connection(std::move(origin))
	{

	}

	bool sync_connection::alive()
	{
		return _context && (make_reply(_context, "PING") || redisReconnect(_context) == REDIS_OK);
	}

	bool sync_connection::send(const std::shared_ptr<procedure>& procedure)
	{
		return procedure && procedure->execute(_context) == procedure::state::success;
	}

	async_connection::async_connection(const std::string& host, const std::uint16_t port, const std::string& name)
	:
		connection(host, port, name)
	{

	}

	async_connection::async_connection(sync_connection&& origin)
	:
		connection(std::move(origin))
	{
		if (!_context) {
			return;
		}
		reply_ptr reply(make_reply(_context, "MULTI"));
		if (!reply || reply->type != REDIS_REPLY_STATUS || strcmp(reply->str, "OK") != 0) {
			redisFree(_context);
			_context = nullptr;
		}
	}

	async_connection::async_connection(async_connection&& origin)
	:
		connection(std::move(origin))
	{

	}

	async_connection::~async_connection()
	{
		for (; !_list.empty(); _list.pop_front()) {
			_list.front()->resolve(nullptr);
		}
	}

	bool async_connection::reconnect()
	{
		for (; !_list.empty(); _list.pop_front()) {
			_list.front()->resolve(nullptr);
		}
		if (!_context) {
			return false;
		} else if (redisReconnect(_context) != REDIS_OK) {
			chaos::log_register<redis::log>::error("connection(", this, ")::reconnect > error ", _context->err, ": ", _context->errstr);
			return false;
		}
		reply_ptr multi_reply(make_reply(_context, "MULTI"));
		if (!multi_reply || multi_reply->type != REDIS_REPLY_STATUS || strcmp(multi_reply->str, "OK") != 0) {
			redisFree(_context);
			_context = nullptr;
			return false;
		}
		return true;
	}

	bool async_connection::alive()
	{
		return send(std::make_shared<ping_command>()) || reconnect();
	}

	bool async_connection::send(const std::shared_ptr<procedure>& procedure)
	{
		if (!_context) {
			procedure->resolve(nullptr);
			return false;
		} else if (procedure->execute(_context) != procedure::state::progress) {
			chaos::log_register<redis::log>::error("async_connection(", this, ")::send > enqueue error");
			return false;
		}
		_list.push_back(procedure);
		return true;
	}

	bool async_connection::send(bool onoff)
	{
		if (!_context) {
			return false;
		} if (onoff) {
			reply_ptr reply(make_reply(_context, "EXEC"));
			if (!reply || _context->err || reply->type == REDIS_REPLY_ERROR) {
				chaos::log_register<redis::log>::error("async_connection(", this, ")::send > exec error ", _context->err, ": ", _context->errstr);
				onoff = false;
			} else {
				for (std::size_t e = 0; e < reply->elements; e++) {
					if (_list.empty()) {
						return true;
					}
					_list.front()->resolve(reply->element[e]);
					_list.pop_front();
				}
			}
		} else {
			reply_ptr reply(make_reply(_context, "DISCARD"));
			if (!reply || _context->err || reply->type == REDIS_REPLY_ERROR) {
				chaos::log_register<redis::log>::error("async_connection(", this, ")::send > discard error ", _context->err, ": ", _context->errstr);
			} else {
				onoff = true;
			}
		}
		for (; !_list.empty(); _list.pop_front()) {
			_list.front()->resolve(nullptr);
		}
		reply_ptr reply(make_reply(_context, "MULTI"));
		if (!reply || reply->type != REDIS_REPLY_STATUS || strcmp(reply->str, "OK") != 0) {
			redisFree(_context);
			_context = nullptr;
			chaos::log_register<redis::log>::error("async_connection(", this, ")::send > multi error ", _context->err, ": ", _context->errstr);
		}
		return onoff;
	}

	bool async_connection::unsend()
	{
		return send(false);
	}
} }
