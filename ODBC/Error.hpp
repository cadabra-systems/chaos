/**
 @file Error.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Error_hpp
#define Chaos_ODBC_Error_hpp

#include "ODBC.hpp"

namespace chaos { namespace odbc {
	class error
	{
	/** @name Constructors */
	/** @{ */
	public:
		error(SQLSMALLINT type, SQLHANDLE handle);
		virtual ~error() = 0;
	/** @} */
	
	/** @name Properties */
	/** @{ */
	private:
		SQLSMALLINT _type;
		SQLHANDLE _handle;
		SQLSMALLINT _i;
		bool _last;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		void reset();
		diagnostic fetch();
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		virtual bool last() final;
	/** @} */
	};
} }

#endif
