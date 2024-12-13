/**
 @file Cursor.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Cursor.hpp"
#include "Log.hpp"
#include "Error/StatementError.hpp"

#include "../Endian.hpp"

namespace chaos { namespace odbc {
	cursor::cursor(const SQLHANDLE connection, bool read_only, std::uint16_t timeout)
	:
		statement(connection, true),
	
		_eof(true),
		_bof(true),
		_empty(true),
		_single(false),
		_bind_offset(0)
	{
		if (read_only) {
			if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_CONCURRENCY, reinterpret_cast<SQLPOINTER>(SQL_CONCUR_READ_ONLY), 0))) {
				chaos::log_register<odbc::log>::error("Cursor(", _statement, ") > Could not set SQL_ATTR_CONCURRENCY");
				return ;
			}
		}
		
		/// @brief Уточить
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_USE_BOOKMARKS, reinterpret_cast<SQLPOINTER>(SQL_UB_VARIABLE), 0))) {
			chaos::log_register<odbc::log>::error("Cursor(", _statement, ") > Could not set SQL_ATTR_USE_BOOKMARKS");
			return ;
		}
		
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_ROW_BIND_OFFSET_PTR, nullptr, 0))) {
			chaos::log_register<odbc::log>::error("Cursor(", _statement, ") > Could not set SQL_ATTR_ROW_BIND_OFFSET_PTR");
			return ;
		}
		
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_ROW_ARRAY_SIZE, reinterpret_cast<SQLPOINTER>(1), 0))) {
			chaos::log_register<odbc::log>::error("Cursor(", _statement, ") > Could not set SQL_ATTR_ROW_ARRAY_SIZE");
			return ;
		}
		
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_ROW_BIND_TYPE, reinterpret_cast<SQLPOINTER>(sizeof(cursor_bookmark)), 0))) {
			chaos::log_register<odbc::log>::error("Cursor(", _statement, ") > Could not set SQL_ATTR_ROW_BIND_TYPE");
			return ;
		}
		
		/// @brief Не нужно никуда(в массив) сообщать статус записей из фетча
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_ROW_STATUS_PTR, nullptr, 0))) {
			chaos::log_register<odbc::log>::error("Cursor(", _statement, ") > Could not SQL_ATTR_ROW_STATUS_PTR");
			return ;
		}
	}
	
	cursor::cursor(cursor&& origin)
	:
		statement(std::move(origin)),
	
		_column_array(origin._column_array),
		_eof(origin._eof),
		_bof(origin._bof),
		_empty(origin._empty),
		_single(origin._single),
		_bookmark(origin._bookmark),
		_bind_offset(origin._bind_offset)
	{
		origin._column_array.clear();		
		origin._eof = true;
		origin._bof = true;
		origin._empty = true;
		origin._single = false;
		origin._bookmark.reset();
		origin._bind_offset = 0;
	}
	
	cursor& cursor::operator=(cursor&& right)
	{
		if (this == &right) {
			return *this;
		}
		
		statement::operator=(std::move(right));
		
		_column_array = std::move(right._column_array);
		_bof = right._bof;
		_eof = right._eof;
		_single = right._single;
		_empty = right._empty;
		_bookmark = right._bookmark;
		_bind_offset = right._bind_offset;
		
		_column_array.clear();
		right._bof = true;
		right._eof = true;
		right._single = false;
		right._empty = true;
		right._bookmark.reset();
		right._bind_offset = 0;
		
		return *this;
	}
	
	cursor::~cursor()
	{
		_column_array.clear();
		if (_statement != SQL_NULL_HANDLE) {
			chaos::log_register<odbc::log>::debug("Cursor(", _statement, ") > All resources associated with the statement are going to to being freed");
			SQLFreeStmt(_statement, SQL_CLOSE);
			SQLFreeStmt(_statement, SQL_UNBIND);
			SQLFreeStmt(_statement, SQL_RESET_PARAMS);
		}
	}
	
	bool cursor::bind()
	{
		if (statement_status::empty != _status) {
			return false;
		}
			
		/// @brief Количество колонок/филдов
		SQLSMALLINT column_count = 0;
		if (!is_success(SQLNumResultCols(_statement, &column_count))) {
			_status = statement_status::empty;
			return false;
		}
		
		/// @brief Длинна для букмарочки
		SQLSMALLINT bookmark_length(0);
		SQLColAttribute(_statement, 0, SQL_DESC_LABEL, nullptr, 0, &bookmark_length, nullptr);
		/// @brief Биндим букмарочку
		if (!is_success(SQLBindCol(_statement, 0, SQL_C_VARBOOKMARK, _bookmark.mark, sizeof(_bookmark.mark), &_bookmark.length))) {
			_status = statement_status::fault;
			return false;
		}
		
		/// @brief Фейковая нулевая колонка-букмарка — соблюдаем правильную индексацию пользовательских колонок с единицы
		_column_array.push_back({0, "_bookmark", column_data_type::cbinary, sizeof(SQLLEN), 0, false});
		
		/// @brief Работаем с полученными колонками
		for (SQLUSMALLINT index=1; index <= column_count; index++) {
			SQLCHAR* name = new SQLCHAR[SQL_MAX_MESSAGE_LENGTH];
			SQLSMALLINT name_length;
			SQLSMALLINT type;
			SQLULEN size;
			SQLSMALLINT digits;
			SQLSMALLINT nullable;
			
			/// @brief Читаем описания колонок
			if (!is_success(SQLDescribeCol(_statement, index, name, SQL_MAX_COLUMN_NAME_LEN, &name_length, &type, &size, &digits, &nullable))) {
				_status = statement_status::fault;
				delete[] name;
				return false;
			}
			
			/// @brief Добавляем в список
			_column_array.push_back({index, std::string((char*)name, name_length), (column_data_type)type, (std::uint32_t)size, digits, (bool)nullable});
			
			delete[] name;
		}
		
		/// @brief Пока все идет по плану
		_status = statement_status::active;

		if (!is_driven()) {
			return true;
		}
		
		/// @brief Есть хоть что-нибудь в наборе?
		if (!fetch(fetch_orientation::first)) {
			_status = statement_status::empty;
			_empty = true;

			return true;
		}

		_empty = _eof;
		if (_empty) {
			_bof = true;
			_status = statement_status::empty;
			
			return true;
		}
		
		/// @brief Всего лишь одна запись?
		if (!fetch(fetch_orientation::next)) {
			if (_eof) {
				_single = true;
			}
		}
		
		if (!fetch(fetch_orientation::first)) {
			_status = statement_status::fault;
			return false;
		}
		
		return true;
	}
	
	bool cursor::direct(const std::string& query)
	{
		if (!statement::direct(query)) {
			return false;
		}
		
		return bind();
	}

	bool cursor::exec()
	{
		if (!statement::exec()) {
			return false;
		}

		return bind();
	}
	
	bool cursor::fetch(fetch_orientation orientation, long offset)
	{
		if (_status != statement_status::active) {
			return false;
		}
		
		return_status retcode(return_status(SQLFetchScroll(_statement, (SQLSMALLINT)orientation, (SQLLEN)0)));
		
		if (return_status::error == retcode) {
			statement_error error(*this);
			diagnostic diagnosis(error.fetch());
			
			if (diagnosis.code == 0) {
				
			}
			// @todo
			return false;
		}
		
		switch (orientation) {
			case fetch_orientation::first:
			case fetch_orientation::last:
				_bof = (retcode == return_status::no_data) ? true : false;
				_eof = (retcode == return_status::no_data) ? true : false;
				break;
				
			case fetch_orientation::next:
			case fetch_orientation::relative:
				_eof = (retcode == return_status::no_data) ? true : false;
				if (_bof && !_empty) {
					_bof = false;
				}
				break;
				
			case fetch_orientation::previous:
				_bof = (retcode == return_status::no_data) ? true : false;
				if (_eof && !_empty) {
					_eof = false;
				}
				break;
				
			default:
				break;
		}

		/// @??? Непонятно для чего было так: return ;
		/// @note ::fetch мог возвращать всегда успех
		/// (is_success(retcode) || return_status::no_data == retcode);
		return is_success(retcode);
	}
	
	bool cursor::shift()
	{
		if (_status != statement_status::active && _status != statement_status::empty) {
			return false;
		}
		
		_bookmark.reset();
		_column_array.clear();
		_bof = true;
		_eof = true;
		_single = false;
		_empty = true;
		_bind_offset = 0;
		
		return_status status(return_status(SQLMoreResults(_statement)));
		
		switch (status) {
			case return_status::success:
				_status = statement_status::empty;
				return bind();

			case return_status::no_data:
				_status = statement_status::empty;
				return false;
				
			default:
				_status = statement_status::fault;
				return false;
		}
	}
	
	bool cursor::remove()
	{
		return remove({get_bookmark()});
	}
	
	bool cursor::remove(const std::list<const cursor_bookmark>& bookmark_list)
	{
		if (bookmark_list.size() <= 0) {
			return false;
		}
		
		SQLULEN count = bookmark_list.size();
		cursor_bookmark* raw_list = new cursor_bookmark[count];
		std::size_t b(0);
		for (auto& bookmark : bookmark_list) {
			raw_list[b++] = bookmark;
		}
		
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)count, 0))) {
			delete[] raw_list;
			return false;
		}
		
		return_status status(return_status(SQLBulkOperations(_statement, SQL_DELETE_BY_BOOKMARK)));
		delete[] raw_list;
		
		/// @brief Возвращаемся к человеческому виду
		if (!is_success(SQLSetStmtAttr(_statement, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)1, 0))) {
			return false;
		}
		
		/// @brief Потоптались на месте и обновили eof, bof флаги
		if (!fetch(fetch_orientation::relative, 0)) {
			return false;
		}
		
		return is_success(status);
	}
	
	bool cursor::operator()(column_position index, char& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (column_data_type::char_t != _column_array.at(index).type) {
			return false;
		}
		
		SQLCHAR* buffer = new SQLCHAR[_column_array.at(index).size + 1];
		SQLLEN length(0);
		
		/// @bug (danilabagroff, 21/01/15) Может возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)_column_array.at(index).index, SQL_C_CHAR, buffer, _column_array.at(index).size + 1, &length))) {
			delete[] buffer;
			
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
			value = 0;
		} else {
			null = false;
			/// @bug (danilabagroff, 21/01/15) Скорей всего, нужно сделать rtrim, так как будут "пробелы" в хвосте
			value = buffer[0];
		}
		
		delete[] buffer;
		
		return true;
	}
	
	bool cursor::operator()(column_position index, std::string& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		SQLSMALLINT type(0);		
		switch (_column_array.at(index).type) {
			case column_data_type::char_t:
			case column_data_type::varchar:
			case column_data_type::longvarchar:
			case column_data_type::timestamp:
				type = SQL_C_CHAR;
				break;
				
			case column_data_type::widechar:
			case column_data_type::varwidechar:
			case column_data_type::longvarwidechar:
				type = SQL_C_WCHAR;
				break;
				
			case column_data_type::varbinary:
			case column_data_type::longvarbinary:
				type = SQL_C_BINARY;
				break;
				
			default:
				return false;
		}
		
		SQLLEN length(0);
		char dummy[1];
		/// @brief Поулчаем фактический размер данных
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)_column_array.at(index).index, SQL_C_CHAR, dummy, 0, &length))) {
			return false;
		}

		/// @note NULL;
		if (SQL_NULL_DATA == length) {
			null = true;
			value.clear();
			return true;
		}

		SQLCHAR* buffer = new SQLCHAR[length + 1]; // _column_array.at(index).size
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)_column_array.at(index).index, type, buffer, length + 1, &length))) {
			delete[] buffer;
			return false;
		}

		null = false;
		/// @bug (danilabagroff, 21/01/15) Скорей всего, нужно сделать rtrim, так как будут "пробелы" в хвосте
		/// @note (danilabagroff, 26/01/17) Только в том случае когда SQL_C_CHAR или SQL_C_WCHAR
		value = std::string(reinterpret_cast<char*>(buffer), length);
		delete[] buffer;
		
		return true;
	}

	bool cursor::operator()(column_position index, flex& value, bool& null)
	{
		std::string dump;
		if (!operator()(index, dump, null)) {
			return false;
		}

		if (null) {
			return true;
		}

		value = flex::from_json(dump);

		return true;
	}
	
	bool cursor::operator()(column_position index, double& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (_column_array.at(index).type != column_data_type::numeric /* было ::real ? */) {
			/// @todo Exception
			return false;
		}

		SQLLEN length(0);
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможный баг с sizeof(std::int32_t) — существует вероятность, что правильней использовать ODBC-шные тайпдефы, а-ля SQLSMALLINT
		/// @bug (danilabagroff, 20/01/16) Может быть SQL_C_SHORT ?

		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_DOUBLE, &value, sizeof(double), &length))) {
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}
		
		return true;
	}

	bool cursor::operator()(column_position index, std::tm& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		SQLLEN length(0);
		if (_column_array.at(index).type == column_data_type::date) {
			SQL_DATE_STRUCT date_struct;
			
			/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
			if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_DATE, &date_struct, sizeof(date_struct), &length))) {
				return false;
			}
			
			if (SQL_NULL_DATA == length) {
				null = true;
			} else {
				value.tm_mon = date_struct.month - 1;
				value.tm_mday = date_struct.day;
				value.tm_year = date_struct.year - 1900;
				value.tm_hour = 0;
				value.tm_min = 0;
				value.tm_sec = 0;
				value.tm_isdst = 0;
				
				null = false;
			}
		} else if (_column_array.at(index).type == column_data_type::timestamp) {
			SQL_TIMESTAMP_STRUCT timestamp_struct;
			if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_TYPE_TIMESTAMP, &timestamp_struct, sizeof(SQL_TIMESTAMP_STRUCT), &length))) {
				return false;
			}
			
			if (SQL_NULL_DATA == length) {
				null = true;
			} else {
				null = false;
				
				value.tm_mon = timestamp_struct.month - 1;
				value.tm_mday = timestamp_struct.day;
				value.tm_year = timestamp_struct.year - 1900;
				value.tm_hour = timestamp_struct.hour;
				value.tm_min = timestamp_struct.minute;
				value.tm_sec = timestamp_struct.second;
				value.tm_isdst = 0;
			}
		} else {
			return false;
		}

		return true;
	}

	bool cursor::operator()(column_position index, chaos::time& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0 || _column_array.at(index).type != column_data_type::timestamp) {
			return false;
		}

		SQLLEN length(0);
		SQL_TIMESTAMP_STRUCT timestamp_struct;		
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_TYPE_TIMESTAMP, &timestamp_struct, sizeof(SQL_TIMESTAMP_STRUCT), &length))) {
			return false;
		}

		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;

			value.set_year(static_cast<std::uint16_t>(timestamp_struct.year));
			value.set_month(static_cast<std::uint8_t>(timestamp_struct.month));
			value.set_day(static_cast<std::uint8_t>(timestamp_struct.day));
			value.set_hour(static_cast<std::uint8_t>(timestamp_struct.hour));
			value.set_minute(static_cast<std::uint8_t>(timestamp_struct.minute));
			value.set_second(static_cast<std::uint8_t>(timestamp_struct.second));
			value.set_fraction(static_cast<std::uint32_t>(timestamp_struct.fraction));
		}

		return true;
	}
	
	bool cursor::operator()(column_position index, std::int8_t& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (_column_array.at(index).type != column_data_type::tinyinteger && _column_array.at(index).type != column_data_type::integer) {
			/// @todo Exception
			return false;
		}

		SQLLEN length(0);
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможный баг с sizeof(std::int32_t) — существует вероятность, что правильней использовать ODBC-шные тайпдефы, а-ля SQLSMALLINT
		/// @bug (danilabagroff, 20/01/16) Может быть SQL_C_SHORT ?
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_TINYINT, &value, sizeof(std::int8_t), nullptr))) {
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}
		
		return true;
	}
	
	bool cursor::operator()(column_position index, std::int16_t& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (_column_array.at(index).type != column_data_type::smallinteger && _column_array.at(index).type != column_data_type::integer) {
			/// @todo Exception
			return false;
		}
		
		SQLLEN length(0);
		
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможный баг с sizeof(std::int32_t) — существует вероятность, что правильней использовать ODBC-шные тайпдефы, а-ля SQLSMALLINT
		/// @bug (danilabagroff, 20/01/16) Может быть SQL_C_SHORT ?
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_SSHORT, &value, sizeof(std::int16_t), &length))) {
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}
		
		return true;
	}
	
	bool cursor::operator()(column_position index, std::int32_t& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (_column_array.at(index).type != column_data_type::integer/* @wtf && _column_array.at(index).type != column_data_type::integer*/) {
			/// @todo Exception
			return false;
		}

		SQLLEN length(0);
		
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможный баг с sizeof(std::int32_t) — существует вероятность, что правильней использовать ODBC-шные тайпдефы, а-ля SQLSMALLINT
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_SLONG, &value, sizeof(std::int32_t), nullptr))) {
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}
		
		return true;
	}
	
	bool cursor::operator()(column_position index, std::int64_t& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (_column_array.at(index).type != column_data_type::biginteger && _column_array.at(index).type != column_data_type::integer) {
			/// @todo Exception
			return false;
		}
		
		SQLLEN length(0);
		
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможный баг с sizeof(std::int32_t) — существует вероятность, что правильней использовать ODBC-шные тайпдефы, а-ля SQLSMALLINT
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_SBIGINT, &value, sizeof(std::int64_t), &length))) {
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}
		
		return true;
	}

	bool cursor::operator()(column_position index, std::uint64_t& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}

		if (_column_array.at(index).type != column_data_type::numeric) {
			/// @todo Exception
			return false;
		}

		SQLLEN length(0);
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможный баг с sizeof(std::int32_t) — существует вероятность, что правильней использовать ODBC-шные тайпдефы, а-ля SQLSMALLINT
		/// @bug (danilabagroff, 20/01/16) Может быть SQL_C_SHORT ?

		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_UBIGINT, &value, sizeof(std::uint64_t), &length))) {
			return false;
		}

		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}

		return true;
	}

	bool cursor::operator()(column_position index, std::array<unsigned char, 16>& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		}
		
		if (_column_array.at(index).type != column_data_type::guid) {
			/// @todo Exception
			return false;
		}

		SQLLEN length(0);
		SQLGUID guid;

		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_GUID, &guid, sizeof(SQLGUID), &length))) {
			return false;
		}
		/// @todo нужно через посредника SQLGUID
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
			guid.Data1 = htonl(guid.Data1);
			guid.Data2 = htons(guid.Data2);
			guid.Data3 = htons(guid.Data3);

			std::copy
			(
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data1)),
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data1)) + sizeof(guid.Data1),
				value.data()
			);
			std::copy
			(
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data2)),
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data2)) + sizeof(guid.Data2),
				value.data() + 4
			);
			std::copy
			(
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data3)),
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data3)) + sizeof(guid.Data3),
				value.data() + 6
			);
			std::copy
			(
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data4)),
				static_cast<const unsigned char*>(static_cast<const void*>(&guid.Data4)) + sizeof(guid.Data4),
				value.data() + 8
			);
		}
		return true;
	}
	
	bool cursor::operator()(column_position index, bool& value, bool& null)
	{
		if (_status != statement_status::active || index <= 0) {
			return false;
		} else if (
					_column_array.at(index).type != column_data_type::bit
					&&
					_column_array.at(index).type != column_data_type::boolean
					&&
					_column_array.at(index).type != column_data_type::char_t
					&&
					_column_array.at(index).type != column_data_type::varchar
		) {
			/// @todo Exception
			return false;
		}

		SQLLEN length(0);
		/// @bug (danilabagroff, 21/01/15) Может? возвращать SQL_STILL_EXECUTING, так как за один раз не смогла получить все данные
		/// @bug (danilabagroff, 29/01/15) Возможно не работает на всех СУБД и стоит уйти в сторону организации
		if (!is_success(SQLGetData(_statement, (SQLUSMALLINT)index, SQL_C_BIT, &value, sizeof(std::uint8_t), &length))) {
			return false;
		}
		
		if (SQL_NULL_DATA == length) {
			null = true;
		} else {
			null = false;
		}
		
		return true;
	}
	
	bool cursor::operator()(column_position index, std::vector<bool>& value, bool& null)
	{
		/// @notimplemented
		return false;
	}
	
	const cursor_bookmark cursor::get_bookmark() const
	{
		return _bookmark;
	}
	
	bool cursor::is_eof() const
	{
		return _eof;
	}
	
	bool cursor::is_bof() const
	{
		return _bof;
	}
	
	bool cursor::is_empty() const
	{
		return _empty;
	}
	
	bool cursor::is_single() const
	{
		return _single;
	}
} }
