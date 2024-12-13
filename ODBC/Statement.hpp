/**
 @file Statement.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Statement_hpp
#define Chaos_ODBC_Statement_hpp

#include "ODBC.hpp"
#include "../StringList.hpp"
#include "../Any.hpp"
#include "../UID.hpp"
#include "../Time.hpp"

#include <string>
#include <list>
#include <set>
#include <utility>
#include <unordered_map>
#include <optional>

namespace chaos { namespace odbc {
	enum class statement_status
	{
		invalid,
		allocated,
		fault,
		empty,
		ready,
		active
	};
	
	class statement_error;
	class cursor_error;
	
	/**
	 @brief 
	 */
	class statement
	{
	friend statement_error;
	friend cursor_error;

	/** @name Classes */
	/** @{ */
	public:
		class binding
		{
		private:
			static const std::unordered_map<column_data_type, bind_data_type> type_map;
			static chaos::any adapt_data(chaos::any&& data, column_data_type type);
			static chaos::any adapt_data(const chaos::uid& data);
			static chaos::any adapt_data(const std::tm& data);
			static chaos::any adapt_data(const std::chrono::system_clock::time_point& data);
			static chaos::any adapt_data(const chaos::time& data);
			static SQLLEN adapt_indicator(column_data_type type);

		public:
			binding(column_data_type remote_data_type, SQLLEN type_size, chaos::any&& value);
			binding(column_data_type remote_data_type, SQLLEN type_size);
			~binding() = default;

		public:
			const bind_data_type local_data_type;
			const column_data_type remote_data_type;
			const SQLULEN type_size;

		private:
			chaos::any data;
			SQLLEN indicator;

		public:
			SQLULEN data_size();
			SQLPOINTER data_ptr();
			SQLLEN* indicator_ptr();
			SQLLEN* indicator_ptr(std::nullptr_t);
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		statement(SQLHANDLE connection, bool driven = true, std::uint16_t timeout = 0);
		statement(statement&& origin);
		statement& operator=(statement&& right);
		
		statement(statement const&) = delete;
		statement& operator=(statement const&) = delete;
		statement& operator=(statement&) = delete;
		statement& operator=(statement const&& right) = delete;

		virtual ~statement();
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
		SQLHSTMT _statement;
		statement_status _status;

	private:
		bool _driven;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	protected:
		virtual bool exec();

	public:
		virtual bool prepare(const std::string& query) final;

		virtual bool exec(const std::string& query) final;
		virtual bool exec(const chaos::string_list& query) final;
		virtual bool exec(std::vector<statement::binding>&& vector) final;

		virtual bool direct(const std::string& query);
		virtual bool direct(const chaos::string_list& query) final;

		virtual bool drive(bool onoff = true) final;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual const statement_status& get_status() const final;
		virtual std::size_t get_affected_rows() const final;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		virtual bool is_active() const final;
		virtual bool is_driven() const final;
	/** @} */
	};
} }

#endif
