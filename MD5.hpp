/**
 @file MD5.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_MD5_hpp
#define Chaos_MD5_hpp

#include <openssl/md5.h>
#include <string>
#include <sstream>
#include <iomanip>

namespace chaos {
	class md5
	{
	public:
		static inline std::string hash(std::string in, const std::string& salt = "")
		{
			unsigned char retval[MD5_DIGEST_LENGTH];
			if (!salt.empty()) {
				in.append(salt);
			}
			MD5(reinterpret_cast<const unsigned char*>(in.data()), in.size(), retval);

			std::stringstream stream;
			stream << std::hex << std::setfill('0');
			for(int c: retval) {
				stream << std::setw(2) << c;
			}

			return stream.str();
		}

		static inline std::size_t length()
		{
			return MD5_DIGEST_LENGTH;
		}
	};
}

#endif
