/**
 @file StatementError.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Error_StatementError_hpp
#define Chaos_ODBC_Error_StatementError_hpp

#include "../Error.hpp"
#include "../Statement.hpp"

namespace chaos { namespace odbc {
	class statement_error : public error
	{
	/** @name Constructors */
	/** @{ */
	public:
		statement_error(const statement& statement);
		virtual ~statement_error() = default;
	/** @} */
	};
} }

#endif
