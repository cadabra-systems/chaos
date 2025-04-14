/**
 @file Statement.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Statement.hpp"
#include "Error/StatementError.hpp"
#include "Log.hpp"

#include "../Flex/ODBC.hpp"

#include <chrono>

namespace chaos { namespace odbc {
	statement::statement(SQLHANDLE connection, bool driven, std::uint16_t timeout)
	:
		_statement(SQL_NULL_HANDLE),
		_status(statement_status::invalid),
		_driven(false)
	{
		if (!is_success(SQLAllocHandle(SQL_HANDLE_STMT, connection, &_statement))) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Could not allocate handle");
			return ;
		} else if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_QUERY_TIMEOUT, reinterpret_cast<SQLPOINTER>(timeout), 0))) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Could not set timeout");
			return ;
		} else if (!drive(driven)) {
			return;
		}
		chaos::log_register<odbc::log>::debug("Statement(", _statement, ") > Handle allocated");
		_status = statement_status::allocated;
	}
	
	statement::statement(statement&& origin)
	:
		_statement(origin._statement),
		_status(origin._status),
		_driven(origin._driven)
	{
		origin._statement = SQL_NULL_HANDLE;
		origin._status = statement_status::invalid;
	}
	
	statement& statement::operator=(statement&& right)
	{
		if (this == &right) {
			return *this;
		}
		
		_statement = std::move(right._statement);
		_status = right._status;
		_driven = right._driven;
		
		right._statement = SQL_NULL_HANDLE;
		right._status = statement_status::invalid;

		return *this;
	}
	
	statement::~statement()
	{
		if (_statement != SQL_NULL_HANDLE) {
			chaos::log_register<odbc::log>::debug("Statement(", _statement, ") > Handle is going to be deallocated");
			SQLFreeHandle(SQL_HANDLE_STMT, _statement);
			_statement = SQL_NULL_HANDLE;
		}
	}

	bool statement::exec()
	{
		const auto start = std::chrono::high_resolution_clock::now();
		chaos::log_register<odbc::log>::debug("Statement(", _statement, ") * Prepared query is executing");
		const SQLRETURN retcode(SQLExecute(_statement));
		if (!is_success(retcode) && SQL_NO_DATA != retcode) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Prepared query execution error: ", retcode);
			_status = statement_status::fault;

			return false;
		}
		_status = statement_status::empty;
		chaos::log_register<odbc::log>::notice("Statement(", _statement, ") > Execution has been successfully completed within ", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count(), " ms.");
		return true;
	}

	bool statement::prepare(const std::string& query)
	{
		chaos::log_register<odbc::log>::debug("Statement(", _statement, ") * Preparing query: \n", query);
		const SQLRETURN retcode(SQLPrepare(_statement, (unsigned char *)query.c_str(), SQL_NTS));
		if (!is_success(retcode)) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Preparation query error: ", retcode);
			_status = statement_status::fault;
			return false;
		}
		_status = statement_status::ready;
		return true;
	}
	
	bool statement::exec(const std::string& query)
	{
		if (!prepare(query) || !exec()) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Prepared execution error\n", query);
			return false;
		}
		return true;
	}
	
	bool statement::exec(const chaos::string_list& query)
	{
		const std::stringstream& stream(query.stream());
		const std::string& str(stream.str());
		return exec(str);
	}

	bool statement::exec(std::vector<statement::binding>&& vector)
	{
		SQLUSMALLINT i(0);
		for (std::vector<statement::binding>::value_type& binding : vector) {
			const SQLRETURN retcode
			(
				SQLBindParameter
				(
					_statement,
					++i,
					SQL_PARAM_INPUT,
					static_cast<SQLSMALLINT>(binding.local_data_type),
					static_cast<SQLSMALLINT>(binding.remote_data_type),
					binding.type_size == 0 ? binding.data_size() : binding.type_size,
					0,
					binding.data_ptr(),
					static_cast<SQLLEN>(binding.data_size()),
					binding.indicator_ptr()
				)
			);
			if (!is_success(retcode)) {
				chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Bind error: ", retcode);
				_status = statement_status::fault;
				return false;
			} else if (binding.data_ptr() == nullptr) { /// < Если нужен NULL, то length == 0, data == nullptr, indicator := SQL_NULL_DATA
				binding.indicator_ptr(nullptr);
			}
		}
		return exec();
	}

	bool statement::direct(const std::string& query)
	{
		const auto start = std::chrono::high_resolution_clock::now();
		chaos::log_register<odbc::log>::debug("Statement(", _statement, ") * Query is executing\n", query);
		const SQLRETURN retcode(SQLExecDirect(_statement, (unsigned char *)query.c_str(), SQL_NTS));
		if (!is_success(retcode) && SQL_NO_DATA != retcode) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Direct execution error: ", retcode, "\n", query);
			_status = statement_status::fault;
			
			return false;
		}
		_status = statement_status::empty;
		chaos::log_register<odbc::log>::notice
		(
			"Statement(", _statement, ") > Execution has been successfully completed within ",
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count(), " ms."
		);
		return true;
	}
	
	bool statement::direct(const chaos::string_list& query)
	{
		const std::stringstream& stream(query.stream());
		const std::string& str(stream.str());
		return direct(str);
	}

	bool statement::drive(bool onoff)
	{
		if (!is_success(SQLSetStmtAttr(_statement, SQL_CURSOR_TYPE, reinterpret_cast<SQLPOINTER>(onoff ? SQL_CURSOR_KEYSET_DRIVEN : SQL_CURSOR_FORWARD_ONLY), 0))) {
			chaos::log_register<odbc::log>::error("Statement(", _statement, ") > Could not set SQL_CURSOR_TYPE");
			return false;
		}
		_driven = onoff;
		return true;
	}

	const statement_status& statement::get_status() const
	{
		return _status;
	}
	
	std::size_t statement::get_affected_rows() const
	{
		SQLLEN retval;
		SQLRowCount(_statement, &retval);
		return retval;
	}
	
	bool statement::is_active() const
	{
		return (_status == statement_status::active || _status == statement_status::empty);
	}

	bool statement::is_driven() const
	{
		return _driven;
	}

	const std::unordered_map<column_data_type, bind_data_type> statement::binding::type_map =
	{
		{column_data_type::tinyinteger, bind_data_type::tinyinteger},
		{column_data_type::smallinteger, bind_data_type::smallinteger},
		{column_data_type::integer, bind_data_type::integer},
		{column_data_type::biginteger, bind_data_type::biginteger},
		{column_data_type::integer, bind_data_type::integer},
		{column_data_type::double_t, bind_data_type::double_t},
		{column_data_type::bit, bind_data_type::bit},
		{column_data_type::boolean, bind_data_type::boolean},
		{column_data_type::guid, bind_data_type::guid}, /// < SQLGUID
		{column_data_type::varbinary, bind_data_type::binary},
		{column_data_type::longvarbinary, bind_data_type::binary},
		{column_data_type::longvarchar, bind_data_type::char_t},
		{column_data_type::date, bind_data_type::date}, ///< SQL_DATE_STRUCT
		{column_data_type::datetime, bind_data_type::timestamp}, ///< SQL_TIMESTAMP_STRUCT
		{column_data_type::timestamp, bind_data_type::timestamp}, ///< SQL_TIMESTAMP_STRUCT
		{column_data_type::timestruct, bind_data_type::timestamp}, ///< SQL_TIMESTAMP_STRUCT
		{column_data_type::char_t, bind_data_type::char_t},
		{column_data_type::varchar, bind_data_type::char_t},
		{column_data_type::widechar, bind_data_type::widechar},
		{column_data_type::varwidechar, bind_data_type::widechar}
	};

	/// @todo Переделать на SFINAE через chaos::any_cast
	chaos::any statement::binding::adapt_data(chaos::any&& data, column_data_type type)
	{
		switch (type)
		{
			case column_data_type::date:
				return adapt_data(static_cast<const std::tm&>(data));

			case column_data_type::datetime:
			case column_data_type::timestamp:
				return adapt_data(static_cast<const std::chrono::system_clock::time_point&>(data));

			case column_data_type::timestruct:
				return adapt_data(static_cast<const chaos::time&>(data));

			case column_data_type::guid:
				return adapt_data(static_cast<const chaos::uid&>(data));

			default:
				return std::move(data);
		}
	}

	chaos::any statement::binding::adapt_data(const chaos::uid& data)
	{
		SQLGUID retval;
		retval.Data1 = static_cast<DWORD>(data[0]) << 24 | static_cast<DWORD>(data[1]) << 16 | static_cast<DWORD>(data[2]) << 8 | static_cast<DWORD>(data[3]);
		retval.Data2 = static_cast<WORD>(data[4]) << 8 | static_cast<WORD>(data[5]);
		retval.Data3 = static_cast<WORD>(data[6]) << 8 | static_cast<WORD>(data[7]);
		retval.Data4[0] = static_cast<BYTE>(data[8]);
		retval.Data4[1] = static_cast<BYTE>(data[9]);
		retval.Data4[2] = static_cast<BYTE>(data[10]);
		retval.Data4[3] = static_cast<BYTE>(data[11]);
		retval.Data4[4] = static_cast<BYTE>(data[12]);
		retval.Data4[5] = static_cast<BYTE>(data[13]);
		retval.Data4[6] = static_cast<BYTE>(data[14]);
		retval.Data4[7] = static_cast<BYTE>(data[15]);
		return retval;
	}

	chaos::any statement::binding::adapt_data(const std::tm& data)
	{
		SQL_DATE_STRUCT retval{0};
		retval.month = data.tm_mon + 1;
		retval.day = data.tm_mday;
		retval.year = data.tm_year + 1900;
		return retval;
	}

	chaos::any statement::binding::adapt_data(const std::chrono::system_clock::time_point& data)
	{
		const std::time_t time(std::chrono::system_clock::to_time_t(data));
		std::tm tm{0};
		gmtime_r(&time, &tm);

		SQL_TIMESTAMP_STRUCT retval{0};
		retval.month = tm.tm_mon + 1;
		retval.day = tm.tm_mday;
		retval.year = tm.tm_year + 1900;
		retval.hour = tm.tm_hour;
		retval.minute = tm.tm_min;
		retval.second = tm.tm_sec;
		return retval;
	}

	chaos::any statement::binding::adapt_data(const chaos::time& data)
	{
		SQL_TIMESTAMP_STRUCT retval{0};
		retval.month = data.get_month();
		retval.day = data.get_day();
		retval.year = data.get_year();
		retval.hour = data.get_hour();
		retval.minute = data.get_minute();
		retval.second = data.get_second();
		retval.fraction = data.get_fraction();
		return retval;
	}

	SQLLEN statement::binding::adapt_indicator(column_data_type type)
	{
		switch (type)
		{
			case column_data_type::date:
				return sizeof(SQL_DATE_STRUCT);

			case column_data_type::timestamp:
			case column_data_type::timestruct:
				return sizeof(SQL_TIMESTAMP_STRUCT);

			case column_data_type::guid:
				return sizeof(SQLGUID);

			case column_data_type::char_t:
			case column_data_type::varchar:
			case column_data_type::longvarchar:
			case column_data_type::widechar:
			case column_data_type::varwidechar:
			case column_data_type::longvarwidechar:
				return SQL_NTS;

			default:
				return 0;
		}
	}

	statement::binding::binding(column_data_type type, SQLLEN size, chaos::any&& anything)
	:
		local_data_type(statement::binding::type_map.at(type)),
		remote_data_type(type),
		type_size(size),
		data(binding::adapt_data(std::move(anything), type)),
		indicator(binding::adapt_indicator(type))
	{

	}

	statement::binding::binding(column_data_type type, SQLLEN size)
	:
		local_data_type(statement::binding::type_map.at(type)),
		remote_data_type(type),
		type_size(size),
		indicator(binding::adapt_indicator(type))
	{

	}

	SQLULEN statement::binding::data_size()
	{
		return static_cast<SQLLEN>(data.size());
	}

	SQLPOINTER statement::binding::data_ptr()
	{
		return data.ptr();
	}

	SQLLEN* statement::binding::indicator_ptr()
	{
		return &indicator;
	}

	SQLLEN* statement::binding::indicator_ptr(std::nullptr_t)
	{
		indicator = SQL_NULL_DATA;
		return &indicator;
	}
} }
