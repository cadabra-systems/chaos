/*
 * Proto.hpp
 *
 *  Created on: 15.03.2011
 *	  Author: danilabagroff
 */

#ifndef Chaos_CMF_Proto_hpp
#define Chaos_CMF_Proto_hpp

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace chaos { namespace cmf {
	enum class version : std::uint8_t
	{
		CMF0 = 0x00,
		CMF1 = 0x01,
		CMF2 = 0x02
	};
		
	enum class value_type : std::uint8_t
	{
		Undefined	= 0x00,
		Null		= 0x01,
		Boolean		= 0x02,
		Integer		= 0x03,
		Number		= 0x04,
		String		= 0x05,
		Date		= 0x06,
		ByteArray	= 0x07,
		Array		= 0x08,
		Object		= 0x09
	};

	enum class object_external_type
	{
		Undefined = 0x99,
		Proplist = 0x00,
		External = 0x01,
		Dynamic = 0x02,
		Proxy = 0x03
	};
	
	class reference_defenition
	{
	public:
		std::string class_name;
		object_external_type encoding;
		std::vector<std::string> property_names;

	public:
		bool operator()(std::string name)
		{
			return (class_name == name);
		}
	};
} }

#endif
