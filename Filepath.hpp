/*
 * Filepath.hpp
 *
 *  Created on: 18.03.2011
 *	  Author: danilabagroff
 */

#ifndef Chaos_Filepath_hpp
#define Chaos_Filepath_hpp

#include <string>
#include <sstream>
#include <iostream>

namespace chaos {
	class filepath final
	{
	/** @name Constructors */
	/** @{ */
	public:
		filepath(const std::string& path, bool auto_location = false);
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		mutable std::istringstream _stream;
		std::string _current;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		const std::string& nip(bool next = true);
		bool forward();
		bool rewind();

		std::string string(bool next = true);
		std::int64_t id(bool next = true);
		std::uint16_t type(bool next = true);
		std::uint32_t size(bool next = true);
		std::uint64_t index(bool next = true);
		std::uintptr_t ptr(bool next = true);
		std::string blob(bool next = true);
	/** @} */
	
	/** @name Operators */
	/** @{ */
	public:
		void operator>>(std::string& value);
		void operator>>(std::uint16_t& value);
		void operator>>(std::int16_t& value);
		void operator>>(std::uint32_t& value);
		void operator>>(std::int32_t& value);
		void operator>>(std::uint64_t& value);
		void operator>>(std::int64_t& value);
		void operator>>(double& value);
		void operator>>(bool& value);
		void operator>>(std::chrono::system_clock::time_point& value);
	/** @} */
		
	/** @name States */
	/** @{ */
	public:
		bool eof() const;
		bool bof() const;
		bool empty() const;
	/** @{ */
	};
}

#endif
