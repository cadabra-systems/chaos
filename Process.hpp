/**
 @file Process.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_Process_hpp
#define Chaos_Process_hpp

#include <string>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

namespace chaos {
	class process
	{
	public:
		static bool set_user(const std::string& name)
		{
			struct passwd* current_password(getpwuid(geteuid()));
			if (current_password) {
				if (name.compare(current_password->pw_name) == 0) {
					return true;
				}
			}
			struct passwd* desired_password(getpwnam(name.c_str()));
			return (desired_password) ? (setuid(desired_password->pw_uid) == 0) : false;
		}

		static bool set_group(const std::string& name)
		{
			struct group* current_group(getgrgid(getegid()));
			if (current_group) {
				if (name.compare(current_group->gr_name) == 0) {
					return true;
				}
			}
			struct group* desired_group(getgrnam(name.c_str()));
			return (desired_group) ? setgid(desired_group->gr_gid) == 0 : false;
		}

		static std::uint32_t get_pid()
		{
			return ::getpid();
		}

		static bool signal(int no)
		{
			return ::kill(::getpid(), no) == 0;
		}

		static void abort()
		{
			std::abort();
		}
	};
}

#endif
