/**
 @file ProgramName.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_ProgramName_hpp
#define Chaos_ProgramName_hpp

#ifdef _WIN32
#define CHAOS_PROGRAMNAME_WIN
#ifdef _WIN64
#define CHAOS_PROGRAMNAME_WIN
#endif
#elif __APPLE__
#define CHAOS_PROGRAMNAME_MAC
#elif __linux
#define CHAOS_PROGRAMNAME_LINUX
#include <errno.h>
#elif __unix
#include <sys/param.h>
#if defined(BSD)
#define CHAOS_PROGRAMNAME_BSD
#elif
#define CHAOS_PROGRAMNAME_UNIX
#endif
#elif __posix
#define CHAOS_PROGRAMNAME_POSIX
#endif

#include <string>

namespace chaos {
	class program_name
	{
	public:
		static std::string get()
		{
#if defined(CHAOS_PROGRAMNAME_BSD) || defined(CHAOS_PROGRAMNAME_MAC)
		return std::string(getprogname());
#elif defined(CHAOS_PROGRAMNAME_LINUX)
		return std::string(program_invocation_short_name);
#else
		return "";
#endif
		}

		static void set(const std::string name)
		{
#if defined(CHAOS_PROGRAMNAME_BSD) || defined(CHAOS_PROGRAMNAME_MAC)
			setprogname(name.c_str());
#elif defined(CHAOS_PROGRAMNAME_LINUX)
			//program_invocation_name = name;
#else
			return;
#endif
		}
	};
}

#endif
