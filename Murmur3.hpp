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
	public:
		static inline std::uint32_t hash(const std::string_view data, std::uint32_t seed = 0)
		{
			if (data.empty()) {
				return 0;
			}

			const std::size_t length(data.length());
			const std::uint8_t* key(reinterpret_cast<const std::uint8_t*>(data.data()));
			std::uint32_t retval = seed;

			if (length > 3) {
				const std::uint32_t* key_x4 = reinterpret_cast<const std::uint32_t*>(key);
				size_t i = length >> 2;
				do {
					uint32_t k = *key_x4++;
					k *= 0xcc9e2d51;
					k = (k << 15) | (k >> 17);
					k *= 0x1b873593;
					retval ^= k;
					retval = (retval << 13) | (retval >> 19);
					retval = (retval * 5) + 0xe6546b64;
				} while (--i);

				key = reinterpret_cast<const std::uint8_t*>(key_x4);
			}

			if (length & 3) {
				std::size_t i(length & 3);
				std::uint32_t k(0);
				key = &key[i - 1];
				do {
				  k <<= 8;
				  k |= *key--;
				} while (i--);

				k *= 0xcc9e2d51;
				k = (k << 15) | (k >> 17);
				k *= 0x1b873593;
				retval ^= k;
			}

			retval ^= length;
			retval ^= retval >> 16;
			retval *= 0x85ebca6b;
			retval ^= retval >> 13;
			retval *= 0xc2b2ae35;
			retval ^= retval >> 16;

			return retval;
		}
	/** @} */
	};
}

#endif

