//
//  TemporaryFile.hpp
//
//  Created by Daniil A Megrabyan on 25.08.15.
//  Copyright (c) 2015 Cadabra Systems. All rights reserved.
//

#ifndef Chaos_TemporaryFile_hpp
#define Chaos_TemporaryFile_hpp

#include "Filesystem.hpp"

#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <string>

namespace chaos {
	class temporary_file
	{
	/** @name Constructors */
	/** @{ */
	public:
		temporary_file(std::string prefix = "temp", bool auto_close = true)
		:
			_path(chaos::filesystem::temp_directory_path(_error) / prefix.append(".XXXXXX")),
			_handle(!_error ? mkstemp(_path.data()) : -1)
		{
			if (auto_close) {
				close();
			}
		}

		temporary_file(temporary_file&& origin)
		:
			_path(std::move(origin._path)),
			_error(std::move(origin._error)),
			_handle(std::move(origin._handle))
		{
			origin._path.clear();
			origin._error.clear();
			origin._handle = -1;
		}

		~temporary_file()
		{
			if (close()) {
				unlink(_path.data());
			}
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	public:
		std::string _path;
		std::error_code _error;
		int _handle;
	/** @} */
	
	/** @name Procedures */
	/** @{ */
	public:
		bool close()
		{
			if (_handle > 0) {
				::close(_handle);
				_handle = -1;
				_error.clear();
				return true;
			}
			return false;
		}

		bool read(std::string& output, bool append = false)
		{
			std::size_t length(size());
			if (length > 0) {
				char* pointer(nullptr);
				if (!append) {
					output.resize(length);
					pointer = output.data();
				} else {
					output.resize(output.length() + length);
					pointer = output.data() + output.length();
				}

				do {
					std::size_t offset(::read(_handle, pointer, 1024));
					if (offset > 0) {
						length -= offset;
						pointer += offset;
					} else if (0 == offset) {
						return true;
					} else {
						return false;
					}
				} while (length > 0);
				return true;
			}
			return false;
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		temporary_file& operator=(temporary_file&& rhs)
		{
			_path = std::move(rhs._path);
			rhs._path.clear();
			_error = std::move(rhs._error);
			rhs._error.clear();
			_handle = std::move(rhs._handle);
			rhs._handle = -1;

			return *this;
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const std::string& path() const
		{
			return _path;
		}

		std::size_t size() const
		{
			if (_handle <= 0) {
				return 0;
			}
			const off_t backup(lseek(_handle, static_cast<size_t>(0), SEEK_CUR));
			const std::size_t retval(lseek(_handle, static_cast<size_t>(0), SEEK_END));
			lseek(_handle, backup, SEEK_SET);
			return retval;
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool valid() const
		{
			return (!_error && _handle >= 0);
		}
	/** @} */
	};
}

#endif
