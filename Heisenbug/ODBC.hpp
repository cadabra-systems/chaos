/**
 @file ODBC.hpp
 @date 15.04.21
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_ODBC_hpp
#define Chaos_Heisenbug_ODBC_hpp

#include "../Heisenbug.hpp"

#include "../ODBC/ODBC.hpp"

namespace chaos {
	class odbc_test : public heisen_test
	{
	/** @name Aliases */
	/** @{ */
	public:
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		odbc_test()
		:
			heisen_test("ODBC")
		{

		};
		virtual ~odbc_test() = default;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	protected:
		virtual void structuralize() override
		{
			HEISEN(V2);
			HEISEN(V3);
		}
	/** @} */
		
	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief
		 */
		void testV2()
		{
			SQLHENV environment(nullptr);
			SQLHDBC connection(nullptr);

			ARE_EQUAL(SQLAllocEnv(&environment), SQL_SUCCESS);
			ARE_EQUAL(SQLAllocConnect(environment, &connection), SQL_SUCCESS);

			std::string connection_string = "Driver={ClickHouse};Server=localhost;Port=8123;Database=temp1;UID=ace;PWD=ace;";
			SQLRETURN retcode = SQLDriverConnect(connection, nullptr, (SQLCHAR*)connection_string.c_str(), SQL_NTSL, nullptr, 0, 0, SQL_DRIVER_NOPROMPT);

			if (environment != nullptr) {
				SQLFreeEnv(&environment);
			}

			if (connection != nullptr) {
				SQLFreeConnect(&connection);
			}
			ARE_EQUAL(retcode, SQL_SUCCESS);
		}

		/**
		 * @brief
		 */
		void testV3()
		{
			SQLHENV environment(nullptr);
			SQLHDBC connection(nullptr);

			ARE_EQUAL(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environment), SQL_SUCCESS);
			ARE_EQUAL(SQLSetEnvAttr(environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*>(SQL_OV_ODBC3), 0), SQL_SUCCESS);
			ARE_EQUAL(SQLAllocHandle(SQL_HANDLE_DBC, environment, &connection), SQL_SUCCESS);

			std::string connection_string = "Driver={ClickHouse};Server=localhost;Port=8123;Database=temp1;UID=ace;PWD=ace";
			SQLRETURN retcode = SQLDriverConnect(connection, nullptr, (SQLCHAR*)connection_string.c_str(), SQL_NTSL, nullptr, 0, 0, SQL_DRIVER_NOPROMPT);

			if (environment != nullptr) {
				SQLFreeHandle(SQL_HANDLE_ENV, environment);
			}

			if (connection != nullptr) {
				SQLFreeHandle(SQL_HANDLE_DBC, connection);
			}
			ARE_EQUAL(retcode, SQL_SUCCESS);
		}
	/** @} */
	
	/** @name Setters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
	/** @} */
		
	/** @name Formatters */
	/** @{ */
	public:
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
	/** @} */
		
	/** @name Hooks */
	/** @{ */
	public:
	/** @} */
		
	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
