/**
 @file Color.cpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#include "Color.hpp"

namespace chaos {
	const std::map<std::string, std::int32_t> color::map =
	{
		{"clear", 0x1000000},
		{"black", 0x000000},
		{"white", 0xFFFFFF},
		{"blue", 0x0000FF},
		{"red", 0xFF0000},
		{"orange", 0xFFA500},
		{"lime", 0x00FF00},
		{"yellow", 0xFFFF00},
		{"cyan", 0x00FFFF},
		{"magenta", 0xFF00FF},
		{"silver", 0xC0C0C0},
		{"gray", 0x808080},
		{"maroon", 0x800000},
		{"olive", 0x808000},
		{"green", 0x008000},
		{"purple", 0x800080},
		{"teal", 0x008080},
		{"navy", 0x000080}
	};
	
	std::int32_t color::get(std::string name)
	{
		std::map<std::string, std::int32_t>::const_iterator c(color::map.find(name));
		if (std::end(color::map) == c) {
			return -1;
		}
		
		return c->second;
	}
}
