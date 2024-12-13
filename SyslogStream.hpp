/**
 @file SyslogStream.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_SyslogStream_hpp
#define Chaos_SyslogStream_hpp

#include "ProgramName.hpp"

#include <iostream>
#include <syslog.h>

namespace chaos {
	struct syslog_streambuf : std::basic_streambuf<char>
	{
		syslog_streambuf()
		{
			::openlog(program_name::get().data(), LOG_PID/* | LOG_PERROR*/, LOG_DAEMON);
		}

		virtual ~syslog_streambuf() override
		{
			::closelog();
		}

		virtual std::basic_streambuf<char>::int_type overflow(std::basic_streambuf<char>::int_type c) override
		{
			return traits_type::not_eof(c);
		}

		virtual std::streamsize xsputn(const char_type* sequence, std::streamsize count) override
		{
			::syslog(LOG_ALERT, "%s", sequence);
			return count;
		}
	};

	inline static thread_local syslog_streambuf syslog_buffer;
	inline static thread_local std::ostream syslog(&syslog_buffer);
}

#endif
