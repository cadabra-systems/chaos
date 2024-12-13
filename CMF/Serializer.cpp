  /*
 * serializer.cpp
 *
 *  Created on: 18.03.2011
 *	  Author: danilabagroff
 */

#include "Serializer.hpp"
#include "../Endian.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

namespace chaos { namespace cmf {
	serializer::serializer(std::ostream& stream)
	:
		abstract_serializer(stream)
	{
		
	}
	
	void serializer::write_type(const value_type value)
	{
		write_byte(static_cast<std::uint8_t>(value));
	}
	
	void serializer::write_string(const std::string& value)
	{
		std::size_t length(value.size());
		
		if (0 == length) {
			write_size(0x00);
			
			return ;
		}

		write_size(length);
		write_bytes(value);
	}
	
	void serializer::write_unsigned_short_integer(const std::uint16_t value)
	{
		char tmp[2] = {static_cast<char>((value >> 8) & 0xFF), static_cast<char>(value & 0xFF)};
		write_bytes(std::string(tmp, 2));
	}
	
	void serializer::write_short_integer(const std::int16_t value)
	{
		char tmp[2] = {static_cast<char>((value >> 8) & 0xFF), static_cast<char>(value & 0xFF)};
		write_bytes(std::string(tmp, 2));
	}
	
	void serializer::write_unsigned_integer(const std::uint32_t value)
	{
		char tmp[4] = {
			static_cast<char>((value >> 24) & 0xFF),
			static_cast<char>((value >> 16) & 0xFF),
			static_cast<char>((value >> 8) & 0xFF),
			static_cast<char>(value & 0xFF)
		};
		write_bytes(std::string(tmp, 4));
	}
	
	void serializer::write_integer(const std::int32_t value)
	{
		char tmp[4] = {
			static_cast<char>((value >> 24) & 0xFF),
			static_cast<char>((value >> 16) & 0xFF),
			static_cast<char>((value >> 8) & 0xFF),
			static_cast<char>(value & 0xFF)
		};
		write_bytes(std::string(tmp, 4));
	}
	
	void serializer::write_var_integer(const std::int32_t value)
	{
		return write_integer(value);
		/// @todo До лучших времен
/*
		if ((value & 0x7fffff80) == 0) {
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7fffc000) == 0) {
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7fe00000) == 0) {
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		writeByte((value >> 22) | 0x80);
		writeByte((value >> 15) | 0x80);
		writeByte((value >> 8) | 0x80);
		writeByte(value & 0xff);
*/
	}
	
	
	void serializer::write_varlong_integer(const std::int64_t value)
	{
		return write_long_integer(value);
		/// @todo До лучших времен
/*
		if ((value & 0x7fffffffffffff80) == 0) {
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7fffffffffffc000) == 0) {
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7fffffffffe00000) == 0) {
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7ffffffff0000000) == 0) {
			writeByte((value >> 21 ) | 0x80);
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7ffffff800000000) == 0) {
			writeByte((value >> 28 ) | 0x80);
			writeByte((value >> 21 ) | 0x80);
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7ffffc0000000000) == 0) {
			writeByte((value >> 35 ) | 0x80);
			writeByte((value >> 28 ) | 0x80);
			writeByte((value >> 21 ) | 0x80);
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		if ((value & 0x7ffe000000000000) == 0) {
			writeByte((value >> 42 ) | 0x80);
			writeByte((value >> 35 ) | 0x80);
			writeByte((value >> 28 ) | 0x80);
			writeByte((value >> 21 ) | 0x80);
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}

		if ((value & 0x7f00000000000000) == 0) {
			writeByte((value >> 49 ) | 0x80);
			writeByte((value >> 42 ) | 0x80);
			writeByte((value >> 35 ) | 0x80);
			writeByte((value >> 28 ) | 0x80);
			writeByte((value >> 21 ) | 0x80);
			writeByte((value >> 14 ) | 0x80);
			writeByte((value >> 7 ) | 0x80);
			writeByte(value & 0x7f);
			
			return ;
		}
		
		writeByte((value >> 56) | 0xFF);
		writeByte((value >> 48) | 0xFF);
		writeByte((value >> 40) | 0xFF);
		writeByte((value >> 32) | 0xFF);
		writeByte((value >> 24) | 0xFF);
		writeByte((value >> 16) | 0xFF);
		writeByte((value >> 8) | 0xFF);
		writeByte(value & 0xff);
*/
	}
	
	void serializer::write_long_integer(const std::int64_t value)
	{
		char tmp[8] = {
			static_cast<char>((value >> 56) & 0xFF),
			static_cast<char>((value >> 48) & 0xFF),
			static_cast<char>((value >> 40) & 0xFF),
			static_cast<char>((value >> 32) & 0xFF),
			static_cast<char>((value >> 24) & 0xFF),
			static_cast<char>((value >> 16) & 0xFF),
			static_cast<char>((value >> 8) & 0xFF),
			static_cast<char>(value & 0xFF)
		};
		write_bytes(std::string(tmp, 8));
	}
	
	void serializer::write_unsigned_long_integer(const std::uint64_t value)
	{
		char tmp[8] = {
			static_cast<char>((value >> 56) & 0xFF),
			static_cast<char>((value >> 48) & 0xFF),
			static_cast<char>((value >> 40) & 0xFF),
			static_cast<char>((value >> 32) & 0xFF),
			static_cast<char>((value >> 24) & 0xFF),
			static_cast<char>((value >> 16) & 0xFF),
			static_cast<char>((value >> 8) & 0xFF),
			static_cast<char>(value & 0xFF)
		};
		write_bytes(std::string(tmp, 8));
	}
	
	void serializer::write_doudle(const double value)
	{
		union aligned {
			double dval;
			char cval[8];
		} data;
		
		const char *number = data.cval;
		data.dval = value;
		
		if (endian::little()) {
			char number_[8] = {number[7], number[6], number[5], number[4], number[3], number[2], number[1], number[0]};
			write_bytes(std::string(number_, 8));
		}
		else {
			write_bytes(std::string(number, 8));
		}
	}
	
	void serializer::write_boolean(const bool yesno)
	{
		write_byte(yesno);
	}
	
	void serializer::write_datetime(const std::chrono::system_clock::time_point& value)
	{
		return write_unsigned_long_integer(std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch()).count());
	}
	
	void serializer::write_id(const std::int64_t id)
	{
		return write_varlong_integer(id);
	}
	
	void serializer::write_size(const std::uint32_t& size)
	{
		write_unsigned_integer(size);
	}
	
	void serializer::write_index(const std::uint64_t& size)
	{
		write_unsigned_long_integer(size);
	}
	
	void serializer::write_blob(const std::string& value)
	{
		write_size(value.size());
		write_bytes(value);
	}
	
	void serializer::operator<<(const std::string& value)
	{
		write_string(value);
	}
	
	void serializer::operator<<(const std::uint8_t& value)
	{
		write_byte(value);
	}
	
	void serializer::operator<<(const std::uint16_t& value)
	{
		write_unsigned_short_integer(value);
	}
	
	void serializer::operator<<(const std::int16_t& value)
	{
		write_short_integer(value);
	}
	
	void serializer::operator<<(const std::uint32_t& value)
	{
		write_unsigned_integer(value);
	}
	
	void serializer::operator<<(const std::int32_t& value)
	{
		write_integer(value);
	}
	
	void serializer::operator<<(const std::uint64_t& value)
	{
		write_unsigned_long_integer(value);
	}
	
	void serializer::operator<<(const std::int64_t& value)
	{
		write_long_integer(value);
	}

	void serializer::operator<<(const double& value)
	{
		write_doudle(value);
	}
	
	void serializer::operator<<(const bool& value)
	{
		write_boolean(value);
	}
	
	void serializer::operator<<(const std::chrono::system_clock::time_point& value)
	{
		write_datetime(value);
	}
} }
