/**
 @file UID.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_UID_hpp
#define Chaos_UID_hpp

#ifdef _WIN32
#defone CHAOS_GUID
#include <objbase.h>
#ifdef _WIN64
//define something for Windows (64-bit only)
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Mac OS X
#define CHAOS_UUID
#include <uuid/uuid.h>
#else
// Unsupported platform
#endif
#elif __linux
#define CHAOS_UUID
#include <uuid/uuid.h>
#elif __unix // all unices not caught above
// Unix
#elif __posix
// POSIX
#endif

#include <iostream>
#include <array>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <string>
#include <iomanip>

namespace chaos {
	class uid
	{
	/** @name Statics */
	/** @{ */
	public:
		const static std::size_t size = 16;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		uid(const std::string& source);
		uid(std::array<unsigned char, uid::size> source);
		uid(uid&& origin);
		uid(const uid& origin);
		uid(bool random);
		uid();
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	public:
		void parse(std::istream& stream);
		void parse(const std::string& source);
		void generate();
		void zero();
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		unsigned char operator[](std::size_t index) const;
		uid& operator=(const uid& origin);
		uid& operator=(uid&& origin);
		bool operator<(const uid& origin) const;
		bool operator<=(const uid& origin) const;
		bool operator>(const uid& origin) const;
		bool operator>=(const uid& origin) const;
		bool operator==(const uid& rhs) const;
		bool operator!=(const uid& rhs) const;
		operator std::array<unsigned char, uid::size>&();
		operator const std::array<unsigned char, uid::size>&() const;
		operator std::string() const;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::array<unsigned char, uid::size> _bytes;
	/** @} */
		
	/** @name Convertors */
	/** @{ */
	private:
		unsigned char convert(char c) const;
	/** @} */
		
	/** @name Factories */
	/** @{ */
	public:
		std::string make_string() const;
		unsigned int murmur_hash() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool is_zero() const;
	/** @} */

	friend void* any_pointer(uid& value);
	};
	
	std::istream& operator>>(std::istream& stream, chaos::uid& uid);

	inline void* any_pointer(uid& value)
	{
		return value._bytes.data();
	}

	inline std::size_t any_size(const uid&)
	{
		return uid::size;
	}
}

namespace std {
	template<>
	class hash<chaos::uid>
	{
	public:
		uintptr_t operator()(const chaos::uid& uid) const;
	};
}

#endif
