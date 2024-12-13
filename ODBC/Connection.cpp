/**
 @file
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 @todo #define по платформе и если Win/Lin, то делать #include <sqlncli.h> (danilabagroff, 16/01/15)
*/

#include "Connection.hpp"

#include "Log.hpp"
#include "Error/ConnectionError.hpp"

namespace chaos { namespace odbc {
	connection::connection(connection_target_type type, const std::string& target)
	:
		_status(connection_status::prepared),
		_target_type(type),
		_target(target),
		_environment(nullptr),
		_connection(nullptr),
		_default_timeout(0)
	{
		if (!is_success(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_environment))) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not allocate environment");
			_status = connection_status::invalid_environment;
			return ;
		}

		/// @brief Работаем через ODBC3.0 интерфейс
		if (!is_success(SQLSetEnvAttr(_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*>(SQL_OV_ODBC3), 0))) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not set SQL_ATTR_ODBC_VERSION");
			_status = connection_status::invalid_environment;
			return ;
		}

		if (!is_success(SQLAllocHandle(SQL_HANDLE_DBC, _environment, &_connection))) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not allocate handle");
			_status = connection_status::invalid_environment;
			return;
		}

		/// @brief Уровень изоляции устанавливаем в UNCOMMITED READ
		if (!is_success(SQLSetConnectAttr(_connection, SQL_ATTR_TXN_ISOLATION, reinterpret_cast<SQLPOINTER>(SQL_TXN_READ_UNCOMMITTED), SQL_IS_UINTEGER)) ) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not set SQL_ATTR_TXN_ISOLATION");
			_status = connection_status::invalid_connection;
			return;
		}
		
		/// @brief Без авто-коммитов
		if (!is_success(SQLSetConnectAttr(_connection, SQL_ATTR_AUTOCOMMIT, reinterpret_cast<SQLPOINTER>(SQL_AUTOCOMMIT_OFF), 0))) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not set SQL_ATTR_AUTOCOMMIT");
			_status = connection_status::invalid_connection;
			return;
		}

		/// @brief Чтение и запись
		if (!is_success(SQLSetConnectAttr(_connection, SQL_ATTR_ACCESS_MODE, reinterpret_cast<SQLPOINTER>(SQL_MODE_READ_WRITE), 0))) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not set SQL_ATTR_ACCESS_MODE");
			_status = connection_status::invalid_connection;
			return;
		}

		/// @brief Тайм-аут при логинах: connect(SQLDriverConnect/SQLConnect)
		if (!is_success(SQLSetConnectAttr(_connection, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER*>(5), 0))) {
			chaos::log_register<odbc::log>::error("Connection(", this, ") > Could not set SQL_LOGIN_TIMEOUT");
			_status = connection_status::invalid_connection;
			return ;
		}

		/// @attention (danilabagroff, 15/01/15) Возможно, необходимо устанавливать только для MicrosoftSQL Server подключений
		/// @attention (danilabagroff, 15/01/15) Возможно, понадобится спец. аргумент в конструктор
/*
		if (is_success(SQLSetConnectAttr(_connection, SQL_COPT_SS_PRESERVE_CURSORS, (SQLPOINTER) SQL_PC_ON, SQL_IS_UINTEGER))) {
			_status = connection_status::invalid_connection;
			return;
		}
*/
	}
	
	connection::~connection()
	{
		disconnect();

		if (_environment != nullptr) {
			SQLFreeHandle(SQL_HANDLE_ENV, _environment);
		}
		
		if (_connection != nullptr) {
			SQLFreeHandle(SQL_HANDLE_DBC, _connection);
		}
	}

	bool connection::ping(bool check_alive) const
	{
		if (_status != connection_status::connected || (check_alive && is_alive())) {
			return false;
		} else {
			SQLHSTMT handle_raw(SQL_NULL_HANDLE);
			if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, _connection, &handle_raw)) {
				const std::unique_ptr<void, std::function<SQLRETURN(SQLHANDLE)>> handle_smart(std::move(handle_raw), std::bind(SQLFreeHandle, SQL_HANDLE_STMT, std::placeholders::_1));
				const SQLRETURN retcode(SQLExecDirect(handle_smart.get(), (unsigned char *)"SELECT NULL", SQL_NTS)); /// @note Nulg(Ping!)
				if (!is_success(retcode)) {
					chaos::log_register<odbc::log>::error("Connection(", this, ") > Nulg: ", retcode);
					return false;
				}
				chaos::log_register<odbc::log>::debug("Connection(", this, ") > Nulg: ", retcode);
				return true;
			}
		}
		return false;
	}
	
	bool connection::connect(const std::string& username, const std::string& password)
	{
		if (_status != connection_status::prepared && _status != connection_status::disconnected) {
			return false;
		}

		SQLRETURN retcode(SQL_ERROR);
		if (_target_type == connection_target_type::connection_string) {
			std::string connection_string = _target + ";UID=" + username + ";PWD=" + password + ";";
			retcode = SQLDriverConnect(_connection, nullptr, (SQLCHAR*)connection_string.c_str(), SQL_NTSL, nullptr, 0, 0, SQL_DRIVER_NOPROMPT);
			chaos::log_register<odbc::log>::debug("Connection(", this, ") * Connecting to: ", connection_string);
		} else {
			retcode = SQLConnect(_connection, (SQLCHAR*)_target.c_str(), _target.length(), (SQLCHAR*)username.c_str(), SQL_NTS, (SQLCHAR*)password.c_str(), SQL_NTS);
			chaos::log_register<odbc::log>::debug("Connection(", this, ") * Connecting to: ", _target);
		}

		if (return_status(retcode) != return_status::success) {
			_status = connection_status::disconnected;
			connection_error error(*this);
			do {
				const diagnostic d(error.fetch());
				if (error.last()) {
					break;
				}
				chaos::log_register<odbc::log>::error("Connection(", this, ") > Connection error[", d.code, ";", d.state, "]: ", d.message);
			} while (true);
			return false;
		}

		_status = connection_status::connected;

		_auth.username = username;
		_auth.password = password;

		SQLCHAR info_buffer[254];
		SQLSMALLINT info_length = 0;

		if (is_success(SQLGetInfo(_connection, SQL_DATABASE_NAME, info_buffer, (SQLSMALLINT)sizeof(info_buffer), &info_length))) {
			_db.name = std::string((char *)info_buffer, info_length);
		}

		if (is_success(SQLGetInfo(_connection, SQL_DBMS_VER, info_buffer, (SQLSMALLINT)sizeof(info_buffer), &info_length))) {
			_dbms.version = std::string((char *)info_buffer, info_length);
		}

		if (is_success(SQLGetInfo(_connection, SQL_DBMS_NAME, info_buffer, (SQLSMALLINT)sizeof(info_buffer), &info_length))) {
			_dbms.name = std::string((char *)info_buffer, info_length);
			_dbms.family = (dbms_map.find(_dbms.name) != dbms_map.end()) ? dbms_map.at(_dbms.name) : dbms_family::unknown;
		}

		if (is_success(SQLGetInfo(_connection, SQL_USER_NAME, info_buffer, (SQLSMALLINT)sizeof(info_buffer), &info_length))) {
			_db.user = std::string((char *)info_buffer, info_length);
		}
		chaos::log_register<odbc::log>::debug("Connection(", this, ") > Connected to ", _db.name, "(", _dbms.name, ";", _dbms.version, ")");
		return true;
	}

	bool connection::reconnect(bool force)
	{
		for (std::int8_t l = 0; l < 5; ++l) {
			if (!ping(false) || force) {
				disconnect();
				chaos::log_register<odbc::log>::warning("Connection(", this, ") * Uplink is lost, reconnecting...");
				force = false;
				/// @note sleep() не нужен, так как и без того будет какой-то timeout
				if (connect(_auth.username, _auth.password)) {
					chaos::log_register<odbc::log>::warning("Connection(", this, ") > Uplink is restored, but all statements were invalidated");
					l -= 1;
				}
			} else {
				return true;
			}
		}
		return false;
	}

	bool connection::disconnect()
	{
		if (!is_connected()) {
			return false;
		}

		SQLDisconnect(_connection);
		_status = connection_status::disconnected;
		
		return true;
	}

	bool connection::commit() const
	{
		return is_success(SQLEndTran(SQL_HANDLE_DBC, _connection, SQL_COMMIT));
	}
	
	bool connection::rollback() const
	{
		return is_success(SQLEndTran(SQL_HANDLE_DBC, _connection, SQL_ROLLBACK));
	}

	cursor connection::make_cursor() const
	{
		return cursor(_connection, true, _default_timeout);
	}
	
	statement connection::make_statement() const
	{
		return statement(_connection, _default_timeout);
	}

	bool connection::set_autocommit(bool onoff) const
	{
		return is_success(SQLSetConnectAttr(_connection, SQL_ATTR_AUTOCOMMIT, reinterpret_cast<SQLPOINTER>(onoff ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF), 0));
	}

	bool connection::switch_autocommit(bool onoff) const
	{
		return (is_autocommit() == onoff) ? set_autocommit(!onoff) : false;
	}

	void connection::set_default_timeout(std::uint16_t value)
	{
		_default_timeout = value;
	}
	
	const dbms_family& connection::get_dbms_family() const
	{
		return _dbms.family;
	}
	
	const dbms_family& connection::get_dbms_family(std::string& version) const
	{
		version = _dbms.version;
		
		return _dbms.family;
	}

	const connection_status& connection::get_status() const
	{
		return _status;
	}

	bool connection::is_connected() const
	{
		return (_status == connection_status::connected);
	}

	bool connection::is_alive() const
	{
		SQLINTEGER value(0);
		if (!is_success(SQLGetConnectAttr(_connection, SQL_ATTR_CONNECTION_DEAD, &value, 0, 0))) {
			return false;
		}
		return (SQL_CD_FALSE ==  value);
	}

	bool connection::is_autocommit() const
	{
		SQLINTEGER value;
		if (!is_success(SQLGetConnectAttr(_connection, SQL_ATTR_AUTOCOMMIT, &value, 0, 0))) {
			return false;
		}
		return (SQL_AUTOCOMMIT_ON == value);
	}
} }
