/*
 * Deserializer.cpp
 *
 *  Created on: 18.03.2011
 *	  Author: danilabagroff
 */

#include "Deserializer.hpp"
#include "../Endian.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>

namespace chaos { namespace cmf {
	deserializer::deserializer(std::istream& stream)
	:
		abstract_deserializer(stream)
	{

	}
	
	value_type deserializer::read_type()
	{
		return value_type(static_cast<std::uint8_t>(read_byte()));
	}
	
	std::string deserializer::read_string()
	{
		std::uint32_t length(read_size());
		
		if (0 == length) {
			return "";
		}

		return read_bytes(length);
	}
	
	std::uint16_t deserializer::read_unsigned_short_integer()
	{
		/// @xxx Как в unsigned_longlong_integer?
		return ((read_byte() << 8) | read_byte());
	}
	
	std::int16_t deserializer::read_short_integer()
	{
		/// @xxx Как в unsigned_longlong_integer?
		return (read_byte() << 8) + read_byte();
	}
	
	std::uint32_t deserializer::read_unsigned_integer()
	{
		/// @xxx Как в unsignedLongLongInteger?
		return (read_byte() << 24) + (read_byte() << 16) + (read_byte() << 8) + read_byte();
	}
	
	std::int32_t deserializer::read_integer()
	{
		/// @xxx Как в unsignedLongLongInteger?
		return (read_byte() << 24) + (read_byte() << 16) + (read_byte() << 8) + read_byte();
	}
	
	std::uint64_t deserializer::read_unsigned_long_integer()
	{
		unsigned char bytes[8] = {read_byte(), read_byte(), read_byte(), read_byte(), read_byte(), read_byte(), read_byte(), read_byte()};
		
		return
			(static_cast<std::uint64_t>(bytes[0]) << 56) |
			(static_cast<std::uint64_t>(bytes[1]) << 48) |
			(static_cast<std::uint64_t>(bytes[2]) << 40) |
			(static_cast<std::uint64_t>(bytes[3]) << 32) |
			(static_cast<std::uint64_t>(bytes[4]) << 24) |
			(static_cast<std::uint64_t>(bytes[5]) << 16) |
			(static_cast<std::uint64_t>(bytes[6]) << 8) |
			(static_cast<std::uint64_t>(bytes[7]));
	}
	
	std::int64_t deserializer::read_long_integer()
	{
		unsigned char bytes[8] = {read_byte(), read_byte(), read_byte(), read_byte(), read_byte(), read_byte(), read_byte(), read_byte()};
		
		return
			(static_cast<std::int64_t>(bytes[0]) << 56) |
			(static_cast<std::int64_t>(bytes[1]) << 48) |
			(static_cast<std::int64_t>(bytes[2]) << 40) |
			(static_cast<std::int64_t>(bytes[3]) << 32) |
			(static_cast<std::int64_t>(bytes[4]) << 24) |
			(static_cast<std::int64_t>(bytes[5]) << 16) |
			(static_cast<std::int64_t>(bytes[6]) << 8) |
			(static_cast<std::int64_t>(bytes[7]));
	}
	
	std::int32_t deserializer::read_var_integer()
	{
		return read_integer();
		/// @todo До лучшей жизни
/*
		std::int32_t retval(0);
		unsigned short count = 1;
		char byte = byte();
		
		while (((byte & 0x80) != 0) && count < 4) {
			retval <<= 7;
			retval |= (byte & 0x7f);
			byte = byte();
			count++;
		}
		if (count < 4) {
			retval <<= 7;
			retval |= byte;
		} else {
			retval <<= 8;
			retval |= byte;
			
			if ((retval & 0x10000000) != 0) {
				retval |= ~0xFFFFFFF;
			}
		}
		
		return retval;
*/
	}
	
	std::int64_t deserializer::read_var_long_integer()
	{
		return read_long_integer();
		/// @todo До лучшей жизни		
/*
		std::int64_t retval(0);
		unsigned short count = 1;
		char byte = byte();
		
		while (((byte & 0x80) != 0) && count < 8) {
			retval <<= 7;
			retval |= (byte & 0x7f);
			byte = byte();
			count++;
		}
		if (count < 8) {
			retval <<= 7;
			retval |= byte;
		} else {
			retval <<= 8;
			retval |= byte;
			
			if ((retval & 0x100000000000000) != 0) {
				retval |= ~0xFFFFFFFFFFFFFF;
			}
		}
		
		return retval;
*/
	}
	
	double deserializer::read_doudle()
	{
		unsigned char bytes[8];
		if(endian::little()) {
			for (int i=7; i>=0; i--) {
				bytes[i] = read_byte();
			}
		} else {
			for (int i=0; i<=7; i++) {
				bytes[i] = read_byte();
			}
		}

		return *(reinterpret_cast<double*>(bytes));
	}
	
	bool deserializer::read_boolean()
	{
		return read_byte();
	}
	
	std::chrono::system_clock::time_point deserializer::read_datetime()
	{
		std::chrono::system_clock::time_point retval((std::chrono::microseconds(read_unsigned_long_integer())));
		
		return retval;
	}
	
	std::int64_t deserializer::read_id()
	{
		return read_var_long_integer();
	}
	
	std::uint32_t deserializer::read_size()
	{
		return read_unsigned_integer();
	}
	
	std::uint64_t deserializer::read_index()
	{
		return read_unsigned_long_integer();
	}
	
	std::string deserializer::read_blob()
	{
		std::uint32_t length(read_size());
		std::string retval(read_bytes(length));
		
		return retval;
	}
	
	void deserializer::operator>>(std::string& value)
	{
		value = read_string();
	}
	
	void deserializer::operator>>(std::uint16_t& value)
	{
		value = read_unsigned_short_integer();
	}
	
	void deserializer::operator>>(std::int16_t& value)
	{
		value = read_short_integer();
	}
	
	void deserializer::operator>>(std::uint32_t& value)
	{
		value = read_unsigned_integer();
	}
	
	void deserializer::operator>>(std::int32_t& value)
	{
		value = read_integer();
	}
	
	void deserializer::operator>>(std::uint64_t& value)
	{
		value = read_unsigned_long_integer();
	}
	
	void deserializer::operator>>(std::int64_t& value)
	{
		value = read_long_integer();
	}

	void deserializer::operator>>(double& value)
	{
		value = read_doudle();
	}
	
	void deserializer::operator>>(bool& value)
	{
		value = read_boolean();
	}
	
	void deserializer::operator>>(std::chrono::system_clock::time_point& value)
	{
		value = read_datetime();
	}
} }
