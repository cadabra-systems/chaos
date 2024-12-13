//
//  IPAddress.cpp
//
//  Created by Daniil A Megrabyan on 26.08.16.
//  Copyright © 2016 Cadabra Systems. All rights reserved.
//

#include "PIDFile.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

namespace chaos {
	pid_t pid_file::exists(const std::string& filename)
	{
		FILE* handle(::fopen(filename.c_str(), "r"));
		if (nullptr == handle) {
			return false;
		}
		char* line(nullptr);
		size_t length(0);
		pid_t pid(0);
		if (::getline(&line, &length, handle) > 0) {
			pid = static_cast<pid_t>(atol(line));
		} else {
			return 0;
		}
		if (::kill(pid, 0) == 0) {
			return pid;
		}
/*
		if (errno == ESRCH) {
			return 0;
		}
*/
		return 0;
	}

	pid_file::pid_file()
	:
		_filename(""),
		_handle(0)
	{

	}

	pid_file::~pid_file()
	{
		close();
	}

	bool pid_file::close()
	{
		if (_handle > 0) {
			::close(_handle);
			_handle = 0;
			return true;
		}

		return false;
	}

	bool pid_file::lock(const std::string& filename)
	{
		if (_handle != 0 || filename.empty()) {
			return false;
		}

		_handle = ::open(filename.c_str(), O_RDWR|O_CREAT, 0600);
		if (_handle <= 0) {
			return false;
		}

		_filename = filename;

		if (::lockf(_handle, F_TLOCK, 0) != 0) {
			return false;
		}
		if (::ftruncate(_handle, 0) != 0) {
			return false;
		}

		const std::string pid(std::to_string(getpid()));
		return (::write(_handle, pid.c_str(), pid.length()) == static_cast<ssize_t>(pid.length()));
	}
}
