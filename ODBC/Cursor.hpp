/**
 @file Cursor.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_ODBC_Cursor_hpp
#define Chaos_ODBC_Cursor_hpp

#include "Statement.hpp"
#include "ODBC.hpp"

#include "../Time.hpp"
#include "../Flex.hpp"
#include <list>
#include <array>
#include <vector>
#include <chrono>

namespace chaos { namespace odbc {
	class statement_error;
	
	class cursor : public statement
	{
	friend statement_error;
		
	/** @name Constructors */
	/** @{ */
	public:
		cursor(const SQLHANDLE connection, bool read_only = true, std::uint16_t timeout = 0);
		cursor(cursor&& origin);
		cursor& operator=(cursor&& right);
		cursor(cursor const&) = delete;
		cursor& operator=(cursor&) = delete;
		cursor& operator=(cursor const&) = delete;
		cursor& operator=(cursor const&&) = delete;

		virtual ~cursor() override;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::vector<column> _column_array;
		bool _eof;
		bool _bof;
		bool _empty;
		bool _single;
		cursor_bookmark _bookmark;
		SQLLEN _bind_offset;
	/** @} */
		
	/** @name Procedures  */
	/** @{ */
	private:
		bool bind();
		
	public:
		using statement::direct;
		virtual bool direct(const std::string& query) override;
		virtual bool exec() override;
		virtual bool fetch(fetch_orientation orientation, long offset = 0) final;
		bool shift();
		virtual bool remove() final;
		virtual bool remove(const std::list<const cursor_bookmark>& bookmark_list) final;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		virtual bool operator()(column_position index, char& value, bool& null) final;
		virtual bool operator()(column_position index, std::string& value, bool& null) final;
		virtual bool operator()(column_position index, flex& value, bool& null) final;
		virtual bool operator()(column_position index, double& value, bool& null) final;
		virtual bool operator()(column_position index, std::tm& value, bool& null) final;
		virtual bool operator()(column_position index, chaos::time& value, bool& null) final;
		virtual bool operator()(column_position index, std::int8_t& value, bool& null) final;
		virtual bool operator()(column_position index, std::int16_t& value, bool& null) final;
		virtual bool operator()(column_position index, std::int32_t& value, bool& null) final;
		virtual bool operator()(column_position index, std::int64_t& value, bool& null) final;
		virtual bool operator()(column_position index, std::uint64_t& value, bool& null) final;
		virtual bool operator()(column_position index, std::array<unsigned char, 16>& value, bool& null) final;
		virtual bool operator()(column_position index, bool& value, bool& null) final;
		virtual bool operator()(column_position index, std::vector<bool>& value, bool& null) final;
	/** @} */
		
	/** @name Setters */
	/** @{ */
	public:
		/* @xxx Not implemented */
		void reset_bookmark();
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const cursor_bookmark get_bookmark() const;
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		virtual bool is_eof() const;
		virtual bool is_bof() const;
		virtual bool is_empty() const;
		virtual bool is_single() const;
	/** @} */
	};
} }

#endif
