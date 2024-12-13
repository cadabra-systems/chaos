/**
 @file OID.hpp
 @date 04.03.16
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#ifndef Chaos_OID_hpp
#define Chaos_OID_hpp

#include <iostream>
#include <array>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <string>
#include <iomanip>

namespace chaos {
	class oid
	{
	/** @name Statics */
	/** @{ */
	public:
		constexpr static std::size_t size = 20;
		constexpr static std::size_t length = 40;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		oid(const std::string& source);
		oid(std::array<unsigned char, oid::size> source);
		oid(oid&& origin);
		oid(const oid& origin);
		oid(bool random);
		oid();
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
		oid& operator=(const oid& origin);
		oid& operator=(oid&& origin);
		bool operator<(const oid& origin) const;
		bool operator>(const oid& origin) const;
		bool operator==(const oid& rhs) const;
		bool operator!=(const oid& rhs) const;
		operator std::array<unsigned char, oid::size>&();
		operator const std::array<unsigned char, oid::size>&() const;
		operator std::string() const;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		std::array<unsigned char, oid::size> _bytes;
	/** @} */
		
	/** @name Convertors */
	/** @{ */
	private:
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

	friend void* any_pointer(oid& value);
	};
	
	std::istream& operator>>(std::istream& stream, chaos::oid& oid);

	inline void* any_pointer(oid& value)
	{
		return value._bytes.data();
	}

	inline std::size_t any_size(const oid&)
	{
		return oid::size;
	}
}

namespace std {
	template<>
	class hash<chaos::oid>
	{
	public:
		uintptr_t operator()(const chaos::oid& oid) const;
	};
}

#endif
