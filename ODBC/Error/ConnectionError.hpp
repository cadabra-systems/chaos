/**
 @file
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Error_ConnectionError_hpp
#define Chaos_ODBC_Error_ConnectionError_hpp

#include "../Error.hpp"
#include "../Connection.hpp"

namespace chaos { namespace odbc {
	class connection_error : public error
	{
	/** @name Constructors */
	/** @{ */
	public:
		connection_error(const connection& connection);
		virtual ~connection_error() = default;
	/** @} */
	};
} }

#endif
