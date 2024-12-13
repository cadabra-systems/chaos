/**
 @file EnvironmentError.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "EnvironmentError.hpp"

namespace chaos { namespace odbc {
	environment_error::environment_error(const connection& connection)
	:
		error(SQL_HANDLE_ENV, connection._environment)
	{
		
	}
} }
