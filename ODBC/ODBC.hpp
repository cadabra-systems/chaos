/**
 @file
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_hpp
#define Chaos_ODBC_hpp

#include <string>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <vector>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <array>
#include <cstdint>

namespace chaos { namespace odbc {
	/**
	 @brief Тип СУБД
	 @details Для учета версии необходимо использовать метод ODBC::Connection::getDBMSFamily(std::string& version)
	 */
	enum class dbms_family : std::uint8_t
	{
		unknown = 0,
		sqlite = 1,
		microsoftsql = 2,
		sybase_anywhere = 3,
		firebird = 4,
		postgresql = 5,
		oracle = 6,
		clickhouse = 7
	};
	
	/**
	 @brief "Клей" для типов СУБД между символьным определниями и enum-перечеслениями
	 @details Символьное представление может быть получено с помощью ODBC-функции SQLGetInfo()
	 */
	const std::map<std::string, dbms_family> dbms_map = {
		{"SQLite", dbms_family::sqlite},
		{"Firebird", dbms_family::firebird},
		{"Sybase Anywhere", dbms_family::sybase_anywhere},
		{"PostgreSQL", dbms_family::postgresql},
		{"Microsoft SQL Server", dbms_family::microsoftsql},
		{"Oracle", dbms_family::oracle},
		{"ClickHouse", dbms_family::clickhouse}
	};
	
	/**
	 @brief Коды возврата ODBC-функций
	 */
	enum class return_status : SQLRETURN
	{
		success = SQL_SUCCESS, /**< Успех */
		warning	= SQL_SUCCESS_WITH_INFO, /**< */
		invalid_handle = SQL_INVALID_HANDLE,  /**< */
		error = SQL_ERROR, /**< Как*/
		no_data = SQL_NO_DATA /**< */
	};
	
	inline bool is_success(const SQLRETURN& in)
	{
		return SQL_SUCCEEDED(in);
	}

	inline bool is_success(const return_status& in)
	{
		return (in == return_status::success || in == return_status::warning);
	}

	enum class connection_status
	{
		prepared,
		connected,
		disconnected,
		invalid_environment,
		invalid_connection
	};
	
	enum class connection_target_type
	{
		datasource_name,
		connection_string
	};
	
	enum class fetch_orientation : SQLSMALLINT
	{
		next = SQL_FETCH_NEXT,
		previous = SQL_FETCH_PRIOR,
		last = SQL_FETCH_LAST,
		first = SQL_FETCH_FIRST,
		absolute = SQL_FETCH_ABSOLUTE,
		relative = SQL_FETCH_RELATIVE,
	};
	
	enum class column_data_type : SQLSMALLINT
	{
		unknown = SQL_UNKNOWN_TYPE,
		bit = SQL_BIT,
		boolean = SQL_BIT,
		char_t = SQL_CHAR,
		widechar = SQL_WCHAR,
		varchar = SQL_VARCHAR,
		varwidechar = SQL_WVARCHAR,
		longvarchar = SQL_LONGVARCHAR,
		longvarwidechar = SQL_WLONGVARCHAR,
		varbinary = SQL_VARBINARY,
		longvarbinary = SQL_LONGVARBINARY,
		binary = SQL_BINARY,
		cbinary = SQL_C_BINARY, /// @???
		decimal = SQL_DECIMAL,
		integer = SQL_INTEGER,
		tinyinteger	= SQL_TINYINT,
		biginteger = SQL_BIGINT,
		smallinteger = SQL_SMALLINT,
		numeric = SQL_NUMERIC,
		floa_t = SQL_FLOAT,
		real = SQL_REAL,
		double_t = SQL_DOUBLE,
		datetime = SQL_DATETIME,
		date = SQL_TYPE_DATE,
		time = SQL_TYPE_TIME,
		timestamp = SQL_TYPE_TIMESTAMP,
		timestruct = -SQL_TYPE_TIMESTAMP,
		guid = SQL_GUID
	};

	enum class bind_data_type : SQLSMALLINT
	{
		unknown = SQL_UNKNOWN_TYPE,
		char_t = SQL_C_CHAR, ///< column_data_type::char_t ::varchar ::longvarchar ::timestamp
		widechar = SQL_C_WCHAR, ///< column_data_type::widechar ::varwidechar ::longvarwidechar
		binary = SQL_C_BINARY, ///< column_data_type::varbinary ::longvarbinary
		date = SQL_C_DATE, ///< column_data_type::date
		timestamp = SQL_C_TYPE_TIMESTAMP, ///< column_data_type::timestamp
		double_t = SQL_C_DOUBLE, ///< column_data_type::double_t
		bit = SQL_C_BIT, ///< column_data_type::bit
		boolean = SQL_C_BIT, ///< column_data_type::boolean
		tinyinteger	= SQL_C_STINYINT,
		smallinteger = SQL_C_SSHORT,
		integer = SQL_C_SLONG,
		biginteger = SQL_C_SBIGINT,
		numeric = SQL_C_NUMERIC,
		guid = SQL_C_GUID
	};

	using column_index = std::uint16_t;
	using column_size = std::uint32_t;

	/**
	 @brief Описание ODBC-колонки
	 */
	struct column
	{
		/**
		 @brief Порядковый номер колонки
		 @details Фактически, начинается с 1, т.к. "нулевая" колонка содержит служебную букмарку
		 */
		column_index index;
		/**
		 @brief Имя колонки
		 */
		std::string name;
		/**
		 @brief Тип данных в колонке
		 */
		column_data_type type;
		/**
		 @brief Максимальный размер(в байтах) каждой записи в этой колонке
		 */
		column_size size;
		/**
		 @brief Точность числа
		 @details Используется в NUMERIC-полях
		 */
		std::int16_t precision;
		/**
		 @brief Допустимость NULL-значений
		 */
		bool nullable;
	};
	
	using column_position = std::vector<column>::size_type;

	/**
	 @brief Букмарка(закладка) на положение курсора
	 @details Может быть использована для возвращения на какую-либо запись/строку SQLFetchScroll.
	 */
	struct cursor_bookmark
	{
		SQLCHAR mark[10];
		SQLLEN length;
		cursor_bookmark()
		:
			mark(""),
			length(0)
		{
			
		}
		
		cursor_bookmark(cursor_bookmark const& origin)
		:
			length(origin.length)
		{
			memcpy(mark, origin.mark, (size_t)origin.length);
		}
		
		cursor_bookmark& operator=(const cursor_bookmark& origin)
		{
			length = origin.length;
			memcpy(mark, origin.mark, (size_t)origin.length);
			
			return *this;
		}
		
		/**
		 @brief Сброс закладки
		 */
		void reset()
		{
			mark[0] = '\0';
			length = 0;
		}
	};

	/**
	 @brief Диагностическая информация для получения сообщений о возникших ошибках
	 @details Получается с помощью SQLGetDiagRec().
	 
	 @see odbc::connection_error
	 @see odbc::connection_error
	 @see odbc::statement_error
	 @see odbc::cursor_error
	 @see odbc::environment_error
	 */
	struct diagnostic
	{
		/**
		 @brief Код
		 */
		const int code;
		/**
		 @brief State
		 */
		const std::string state;
		/**
		 @brief Сообщение
		 */
		const std::string message;
	};

	/**
	 * @brief Для statement::exec
	 *
	 */
	struct column_binding
	{
		column_data_type remote_type;
		bind_data_type local_type;
		SQLLEN type_size;
		SQLLEN value_size;
		SQLPOINTER value;
	};
} }

constexpr bool operator==(const tagSQLGUID& lhs, const tagSQLGUID& rhs)
{
	return lhs.Data1 == rhs.Data1 && lhs.Data2 == rhs.Data2 && lhs.Data3 == rhs.Data3 && lhs.Data4 == rhs.Data4;
}
constexpr bool operator!=(const tagSQLGUID& lhs, const tagSQLGUID& rhs)
{
	return lhs.Data1 != rhs.Data1 || lhs.Data2 != rhs.Data2 || lhs.Data3 != rhs.Data3 || lhs.Data4 != rhs.Data4;
}
constexpr bool operator<(const tagSQLGUID& lhs, const tagSQLGUID& rhs)
{
	return false;
}
constexpr bool operator<=(const tagSQLGUID& lhs, const tagSQLGUID& rhs)
{
	return lhs < rhs || lhs == rhs;
}
constexpr bool operator>(const tagSQLGUID& lhs, const tagSQLGUID& rhs)
{
	return false;
}
constexpr bool operator>=(const tagSQLGUID& lhs, const tagSQLGUID& rhs)
{
	return lhs > rhs || lhs == rhs;
}

constexpr bool operator==(const tagDATE_STRUCT& lhs, const tagDATE_STRUCT& rhs)
{
	return lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day;
}
constexpr bool operator!=(const tagDATE_STRUCT& lhs, const tagDATE_STRUCT& rhs)
{
	return lhs.year != rhs.year || lhs.month != rhs.month || lhs.day == rhs.day;
}
constexpr bool operator<(const tagDATE_STRUCT& lhs, const tagDATE_STRUCT& rhs)
{
	return false;
}
constexpr bool operator<=(const tagDATE_STRUCT& lhs, const tagDATE_STRUCT& rhs)
{
	return lhs < rhs || lhs == rhs;
}
constexpr bool operator>(const tagDATE_STRUCT& lhs, const tagDATE_STRUCT& rhs)
{
	return false;
}
constexpr bool operator>=(const tagDATE_STRUCT& lhs, const tagDATE_STRUCT& rhs)
{
	return lhs > rhs || lhs == rhs;
}

constexpr bool operator==(const tagTIMESTAMP_STRUCT& lhs, const tagTIMESTAMP_STRUCT& rhs)
{
	return lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day && lhs.hour == rhs.hour && lhs.minute == rhs.minute && lhs.second == rhs.second && lhs.fraction == rhs.fraction;
}
constexpr bool operator!=(const tagTIMESTAMP_STRUCT& lhs, const tagTIMESTAMP_STRUCT& rhs)
{
	return lhs.year != rhs.year || lhs.month != rhs.month || lhs.day != rhs.day || lhs.hour != rhs.hour || lhs.minute != rhs.minute || lhs.second != rhs.second || lhs.fraction != rhs.fraction;
}
constexpr bool operator<(const tagTIMESTAMP_STRUCT& lhs, const tagTIMESTAMP_STRUCT& rhs)
{
	return false;
}
constexpr bool operator<=(const tagTIMESTAMP_STRUCT& lhs, const tagTIMESTAMP_STRUCT& rhs)
{
	return lhs < rhs || lhs == rhs;
}
constexpr bool operator>(const tagTIMESTAMP_STRUCT& lhs, const tagTIMESTAMP_STRUCT& rhs)
{
	return false;
}
constexpr bool operator>=(const tagTIMESTAMP_STRUCT& lhs, const tagTIMESTAMP_STRUCT& rhs)
{
	return lhs > rhs || lhs == rhs;
}
#endif
