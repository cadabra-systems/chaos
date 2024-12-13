/**
 @file ODBC.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Flex_ODBC_hpp
#define Chaos_Flex_ODBC_hpp

#include "../Flex.hpp"

#include "../String.hpp"

#include <sqltypes.h>

namespace chaos {
	void to_flex(chaos::flex& output, const SQLGUID& input)
	{
		std::string value('0', 16);
		std::copy(
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data1)),
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data1)) + sizeof(input.Data1),
					value.data()
		);
		std::copy(
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data2)),
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data2)) + sizeof(input.Data2),
					value.data() + 4
		);
		std::copy(
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data3)),
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data3)) + sizeof(input.Data3),
					value.data() + 6
		);
		std::copy(
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data4)),
					static_cast<const unsigned char*>(static_cast<const void*>(&input.Data4)) + sizeof(input.Data4),
					value.data() + 8
		);
		output = value;
	}
	void from_flex(const chaos::flex& input, SQLGUID& output)
	{
		/// @todo not implemented
	}

	void to_flex(chaos::flex& output, const SQL_DATE_STRUCT& input)
	{
		output = string::formatted_string("%d-%01d-%01d", input.year, input.month, input.day);
	}
	void from_flex(const chaos::flex& input, SQL_DATE_STRUCT& output)
	{
		/// @todo not implemented
	}

	void to_flex(chaos::flex& output, const SQL_TIMESTAMP_STRUCT& input)
	{
		output = string::formatted_string("%d-%01d-%01d %01d:%01d:%01d.%03d", input.year, input.month, input.day, input.hour, input.minute, input.second, input.fraction);
	}
	void from_flex(const chaos::flex& input, SQL_TIMESTAMP_STRUCT& output)
	{
		/// @todo not implemented
	}
}

#endif
