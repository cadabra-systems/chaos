/**
 @file Murmur3.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Murmur3_hpp
#define Chaos_Murmur3_hpp

#include <string>
#include <cstdint>

namespace chaos {
	class murmur3
	{
	/** @name Statics */
	/** @{ */
	private:
		static constexpr std::uint32_t c1 = 0xcc9e2d51;
		static constexpr std::uint32_t c2 = 0x1b873593;
		static constexpr std::uint32_t r1 = 15;
		static constexpr std::uint32_t r2 = 13;
		static constexpr std::uint32_t m = 5;
		static constexpr std::uint32_t n = 0xe6546b64;

	public:
		static std::uint32_t hash32(const void* key, int length, std::uint32_t seed = 0)
		{
			std::uint32_t retval(seed);

			const std::uint8_t* data(static_cast<const std::uint8_t*>(key));

			const int nblocks(length / 4);
			const std::uint32_t* blocks(reinterpret_cast<const std::uint32_t*>(data));
			for (int i = 0; i < nblocks; i++) {
				std::uint32_t k = blocks[i];

				k *= c1;
				k = (k << r1) | (k >> (32 - r1)); /// < rotleft
				k *= c2;

				retval ^= k;
				retval = (retval << r2) | (retval >> (32 - r2)); /// < rotleft
				retval = retval * m + n;
			}

			std::uint32_t k1(0);
			const std::uint8_t* tail(data + nblocks * 4);
			switch (length & 3) {
				case 3:
					k1 ^= tail[2] << 16;
					[[fallthrough]];

				case 2:
					k1 ^= tail[1] << 8;
					[[fallthrough]];

				case 1:
					k1 ^= tail[0];
					k1 *= c1;
					k1 = (k1 << r1) | (k1 >> (32 - r1));
					k1 *= c2;
					retval ^= k1;
					break;
			}

			retval ^= length;
			/// @note fmix32
			retval ^= retval >> 16;
			retval *= 0x85ebca6b;
			retval ^= retval >> 13;
			retval *= 0xc2b2ae35;
			retval ^= retval >> 16;
			return retval;
		}

		inline static std::uint32_t hash32(const std::string& input, std::uint32_t seed = 0)
		{
			return hash32(input.c_str(), input.length(), seed);
		}

		inline static std::uint32_t hash32(const char* input, std::uint32_t seed = 0)
		{
			return hash32(input, std::strlen(input), seed);
		}
	/** @} */
	};
}

#endif

