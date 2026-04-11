/**
 @file Machine.hpp
 @date 2016-03-04
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Machine_hpp
#define Chaos_Machine_hpp

#include "UID.hpp"

#if defined(__linux__)
#	include <cstdio>
#	include <cstring>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#	include <sys/types.h>
#	include <sys/sysctl.h>
#endif

namespace chaos {
	class machine
	{
	/** @name Statics */
	/** @{ */
	public:
		static uid id()
		{
#if defined(__linux__)
			char buffer[33] = {};
			FILE* file(fopen("/etc/machine-id", "r"));
			if (!file) {
				file = fopen("/var/lib/dbus/machine-id", "r");
			}
			if (!file) {
				return uid{};
			} else if (!fgets(buffer, sizeof(buffer), file)) {
				fclose(file);
				return uid{};
			}
			fclose(file);
			buffer[strcspn(buffer, "\n")] = '\0';
			char buf[37] = {};
			snprintf(buf, sizeof(buf), "%.8s-%.4s-%.4s-%.4s-%.12s", buffer, buffer + 8, buffer + 12, buffer + 16, buffer + 20);
			return uid(std::string(buf));
#elif defined(__APPLE__)
			char buffer[37] = {};
			std::size_t size(sizeof(buffer));
			if (sysctlbyname("kern.uuid", buffer, &size, nullptr, 0) != 0) {
				return uid{};
			}
			return uid(std::string(buffer));
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
			char buffer[37] = {};
			std::size_t size(sizeof(buffer));
			if (sysctlbyname("kern.hostuuid", buffer, &size, nullptr, 0) != 0) {
				return uid{};
			}
			return uid(std::string(buffer));
#else
			return uid{};
#endif
		}
	/** @} */
	};
}

#endif
