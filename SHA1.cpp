/**
 @file SHA1.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "SHA1.hpp"

namespace chaos {
	inline unsigned int sha1::rotate_byte_order(unsigned int value, unsigned int steps)
	{
		return ((value << steps) | (value >> (32 - steps)));
	}

	inline void sha1::clear(unsigned int* buffer)
	{
		for (int pos = 16; --pos >= 0;) {
			buffer[pos] = 0;
		}
	}
			
	void sha1::rotate(unsigned int* result, unsigned int* w)
	{
		unsigned int a = result[0];
		unsigned int b = result[1];
		unsigned int c = result[2];
		unsigned int d = result[3];
		unsigned int e = result[4];
				
		int round = 0;
				
		while (round < 16) {
			const unsigned int t = rotate_byte_order(a, 5) + ((b & c) | (~b & d)) + e + 0x5a827999 + w[round];
			e = d; d = c; c = rotate_byte_order(b, 30); b = a; a = t;
			
			++round;
		}
		
		while (round < 20) {
			w[round] = rotate_byte_order((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			const unsigned int t = rotate_byte_order(a, 5) + ((b & c) | (~b & d)) + e + 0x5a827999 + w[round];
			e = d; d = c; c = rotate_byte_order(b, 30); b = a; a = t;

			++round;
		}
		
		while (round < 40) {
			w[round] = rotate_byte_order((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			const unsigned int t = rotate_byte_order(a, 5) + (b ^ c ^ d) + e + 0x6ed9eba1 + w[round];
			e = d; d = c; c = rotate_byte_order(b, 30); b = a; a = t;

			++round;
		}
		
		while (round < 60) {
			w[round] = rotate_byte_order((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			const unsigned int t = rotate_byte_order(a, 5) + ((b & c) | (b & d) | (c & d)) + e + 0x8f1bbcdc + w[round];
			e = d; d = c; c = rotate_byte_order(b, 30); b = a; a = t;

			++round;
		}
		
		while (round < 80) {
			w[round] = rotate_byte_order((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
			const unsigned int t = rotate_byte_order(a, 5) + (b ^ c ^ d) + e + 0xca62c1d6 + w[round];
			e = d; d = c; c = rotate_byte_order(b, 30); b = a; a = t;

			++round;
		}
				
		result[0] += a;
		result[1] += b;
		result[2] += c;
		result[3] += d;
		result[4] += e;
	}
		
	void sha1::hash(const void* src, int byte_length, unsigned char* hash)
	{
		unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
		const unsigned char* src_array = (const unsigned char*) src;
		unsigned int w[80];
			
		const int end_of_full_blocks = byte_length - 64;
		int end_current_block;
		int current_block = 0;
			
		while (current_block <= end_of_full_blocks) {
			end_current_block = current_block + 64;
				
			for (int round_pos = 0; current_block < end_current_block; current_block += 4) {
				w[round_pos++] = (unsigned int) src_array[current_block + 3] | (((unsigned int) src_array[current_block + 2]) << 8) | (((unsigned int) src_array[current_block + 1]) << 16) | (((unsigned int) src_array[current_block]) << 24);
			}
			rotate(result, w);
		}
			
		end_current_block = byte_length - current_block;
		clear(w);
		int last_block_bytes = 0;
		for (;last_block_bytes < end_current_block; ++last_block_bytes) {
			w[last_block_bytes >> 2] |= (unsigned int) src_array[last_block_bytes + current_block] << ((3 - (last_block_bytes & 3)) << 3);
		}
		w[last_block_bytes >> 2] |= 0x80 << ((3 - (last_block_bytes & 3)) << 3);
		if (end_current_block >= 56) {
			rotate(result, w);
			clear(w);
		}
		w[15] = byte_length << 3;
		rotate(result, w);
			
		for (int hash_byte = 20; --hash_byte >= 0;) {
			hash[hash_byte] = (result[hash_byte >> 2] >> (((3 - hash_byte) & 0x3) << 3)) & 0xff;
		}
	}

	inline std::array<unsigned char, 20> sha1::hash(const void* src, int byte_length)
	{
		std::array<unsigned char, 20> retval;
		hash(src, byte_length, retval.data());
		return retval;
	}
}
