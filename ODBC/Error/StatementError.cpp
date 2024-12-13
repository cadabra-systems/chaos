/**
 @file StatementError.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "StatementError.hpp"

namespace chaos { namespace odbc {
	statement_error::statement_error(const statement& statement)
	:
		error(SQL_HANDLE_STMT, statement._statement)
	{
		
	}
} }
