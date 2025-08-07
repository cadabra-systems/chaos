/**
 @file Connection.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Connection_hpp
#define Chaos_ODBC_Connection_hpp

#include "ODBC.hpp"
#include "Statement.hpp"
#include "Cursor.hpp"

#include <memory>
#include <string>

namespace chaos { namespace odbc {
	class connection_error;
	class environment_error;
	
	class connection
	{
	friend connection_error;
	friend environment_error;

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		/**
		 * 
		 * (MySQL) Driver={MySQL UNICODE Driver};Server=host;Port=3306;Database=myDataBase;Option=3;
		 * (PostgreSQL) Driver={PostgreSQL UNICODE};Server=host;Port=5432;Database=myDataBase;
		 * (Firebird) Driver={Firebird/InterBase};DBName=C:\database\myData.fdb;
		 */
		connection(connection_target_type type, const std::string& target, bool trace = false);
		connection(connection const&) = delete;
		connection& operator=(connection const&) = delete;
		~connection();
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		connection_status _status;
		const connection_target_type _target_type;
		const std::string _target;

		SQLHENV _environment;
		SQLHDBC _connection;
		
		struct {
			std::string name;
			std::string version;
			dbms_family family;
		} _dbms;
		
		struct {
			std::string name;
			std::string user;
		} _db;

		struct {
			std::string username;
			std::string password;
		} _auth;

		std::uint16_t _default_timeout;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	public:
		bool ping(bool check_alive = true) const;
		bool connect(const std::string& username, const std::string& password);
		bool reconnect(bool force = false);
		bool disconnect();
		bool commit() const;
		bool rollback() const;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
		cursor make_cursor() const;
		statement make_statement() const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		bool set_autocommit(bool onoff) const;
		bool switch_autocommit(bool onoff) const;
		void set_default_timeout(std::uint16_t value);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const dbms_family& get_dbms_family() const;
		const dbms_family& get_dbms_family(std::string& version) const;
		const connection_status& get_status() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool is_connected() const;
		bool is_alive() const;
		bool is_autocommit() const;
	/** @} */
	};
} }

#endif
