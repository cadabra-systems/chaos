/**
 @file
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Endian_hpp
#define Chaos_Endian_hpp

#include <arpa/inet.h>
#include <cstdint>

#if defined(__linux__)
#  include <netinet/in.h>
#  include <endian.h>
#  define ntohll(x) be64toh(x)
#elif defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/endian.h>
#  define ntohll(x) be64toh(x)
#elif defined(__OpenBSD__)
#  include <sys/types.h>
#  define ntohll(x) betoh64(x)
#endif

namespace chaos {
	class endian
	{
	public:
		static bool little()
		{
			return (htonl(47) == 47);
		};
		
		static bool big()
		{
			return (htonl(47) != 47);
		};
		
		static bool pdp()
		{
			return false;
		}		
	};
}
#endif
