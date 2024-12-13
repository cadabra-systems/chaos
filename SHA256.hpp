/**
 @file SHA256.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_SHA256_hpp
#define Chaos_SHA256_hpp

#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace chaos {
	class sha256
	{
	public:
		static inline std::string hash(const std::string& input)
		{
			unsigned char retval[SHA256_DIGEST_LENGTH];
			SHA256_CTX context;
			SHA256_Init(&context);
			SHA256_Update(&context, input.c_str(), input.size());
			SHA256_Final(retval, &context);

			std::stringstream stream;
			for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
				stream << std::hex << std::setw(2) << std::setfill('0') << (int)retval[i];
			}
			return stream.str();
		}

		static inline std::size_t length()
		{
			return SHA256_DIGEST_LENGTH;
		}
	};
}

#endif

