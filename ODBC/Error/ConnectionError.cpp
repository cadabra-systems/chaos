/**
 @file
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/
#include "ConnectionError.hpp"

namespace chaos { namespace odbc {
	connection_error::connection_error(const connection& connection)
	:
		error(SQL_HANDLE_DBC, connection._connection)
	{
		
	}
} }
