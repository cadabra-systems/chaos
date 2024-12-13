/**
 @file EnvironmentError.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Error_EnvironmentError_hpp
#define Chaos_ODBC_Error_EnvironmentError_hpp

#include "../Error.hpp"
#include "../Connection.hpp"

namespace chaos { namespace odbc {
	class environment_error : public error
	{
	/** @name Constructors */
	/** @{ */
	public:
		environment_error(const connection& connection);
		virtual ~environment_error() = default;
	/** @} */
	};
} }

#endif
