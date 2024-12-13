/**
 @file SHA1.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 @todo Закончить документированное
*/

#ifndef Chaos_SHA1_hpp
#define Chaos_SHA1_hpp

#include <array>

namespace chaos {
	class sha1
	{
	public:
		inline static void clear(unsigned int* buffer);
		inline static unsigned int rotate_byte_order(unsigned int value, unsigned int steps);
		static void rotate(unsigned int* result, unsigned int* w);
		static void hash(const void* src, int byte_length, unsigned char* hash);
		static inline std::array<unsigned char, 20> hash(const void* src, int byte_length);
	};
}

#endif
