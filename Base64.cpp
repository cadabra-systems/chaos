/**
 @file Base64.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "Base64.hpp"
#include <iostream>

namespace chaos {
	const std::string base64::chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	inline bool base64::valid(unsigned char c)
	{
	  return (isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string base64::encode(const std::string& data)
	{
		return encode(reinterpret_cast<const unsigned char*>(data.c_str()), data.length());
	}

	std::string base64::encode(unsigned char const* data, size_t length)
	{
		std::string retval;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (length--) {
			char_array_3[i++] = *(data++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for(i = 0; (i <4) ; i++) {
					retval += chars[char_array_4[i]];
				}
				i = 0;
			}
		}

		if (i) {
			for(j = i; j < 3; j++) {
				char_array_3[j] = '\0';
			}

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++) {
				retval += chars[char_array_4[j]];
			}
		
			while((i++ < 3)) {
				retval += '=';
			}
		}

		return retval;
	}

	std::string base64::decode(std::string const& data)
	{
		size_t length = data.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string retval;

		while (length-- && ( data[in_] != '=') && base64::valid(data[in_])) {
			char_array_4[i++] = data[in_];
			in_++;
			if (i == 4) {
				for (i = 0; i <4; i++) {
					char_array_4[i] = chars.find(char_array_4[i]);
				}

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++) {
					retval += char_array_3[i];
				}
				i = 0;
			}
		}

		if (i) {
			for (j = i; j <4; j++) {
				char_array_4[j] = 0;
			}

			for (j = 0; j <4; j++) {
				char_array_4[j] = chars.find(char_array_4[j]);
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) {
				retval += char_array_3[j];
			}
		}

		return retval;
	}
}
