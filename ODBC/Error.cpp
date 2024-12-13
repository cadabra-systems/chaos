/**
 @file Error.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Error.hpp"

namespace chaos { namespace odbc {
	error::error(SQLSMALLINT type, SQLHANDLE handle)
	:
		_type(type),
		_handle(handle),
		_i(0),
		_last(false)
	{

	}
	
	error::~error()
	{
		
	}

	void error::reset()
	{
		_i = 0;
		_last = false;
	}

	diagnostic error::fetch()
	{
		if (_last) {
			return {
					0,
					"",
					"Diagnostic record is flushed"
			};
		} else if (nullptr == _handle) {
			_last = true;
			return {
					0,
					"",
					"Handle is missed"
			};
		}

		std::array<SQLCHAR, 6> state;
		std::array<SQLCHAR, SQL_MAX_MESSAGE_LENGTH> message;
		SQLSMALLINT message_length;
		SQLINTEGER code(0);

		const SQLRETURN retcode(SQLGetDiagRec(_type, _handle, ++_i, state.data(), &code, message.data(), SQL_MAX_MESSAGE_LENGTH, &message_length));
		switch (retcode) {
			case SQL_SUCCESS:
			case SQL_SUCCESS_WITH_INFO:
				return {
						code,
						std::string(reinterpret_cast<char*>(state.data())),
						std::string(reinterpret_cast<char*>(message.data()), message_length)
				};

			case SQL_NO_DATA:
				_last = true;
				return {
						0,
						"",
						"Record number was greater than the number of diagnostic records that existed for the handle specified in handle"
				};

			case SQL_ERROR:
				_last = true;
				return {
						1,
						"01000",
						"Record number was less than or equals to zero or buffer length was less than zero"
				};

			case SQL_INVALID_HANDLE:
				_last = true;
				return {
						2,
						"01000",
						"Handle is invalid"
				};

		}

		return {
				3,
				"01000",
				"Unknown diag error"
		};
	}
	
	bool error::last()
	{
		return _last;
	}
} }
