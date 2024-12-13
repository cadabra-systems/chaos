//
//  OID.cpp
//
//  Created by Daniil A Megrabyan on 26.08.16.
//  Copyright © 2016 Cadabra Systems. All rights reserved.
//

#include "OID.hpp"

namespace chaos {
	oid::oid(const std::string& source)
	{
		parse(source);
	}

	oid::oid(std::array<unsigned char, oid::size> source)
	:
		_bytes(source)
	{

	}

	oid::oid(oid&& origin)
	:
		_bytes(std::move(origin._bytes))
	{

	}

	oid::oid(const oid& origin)
	:
		_bytes(origin._bytes)
	{

	}

	oid::oid(bool random)
	{
		if (random) {
			generate();
		} else {
			zero();
		}
	}

	oid::oid()
	:
		_bytes({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
	{

	}

	void oid::generate()
	{
		/// @todo sha1 for current timestamp?
	}

	void oid::parse(std::istream& stream)
	{
		std::string part;
		stream >> part;
		parse(part);
	}

	void oid::parse(const std::string& source)
	{
		if (source.length() > oid::length) {
			return ;
		}

		constexpr std::array<signed char, 256> hex = {
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 00 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 10 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 20 */
			 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, /* 30 */
			-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 40 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 50 */
			-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 60 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 70 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 80 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 90 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* a0 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* b0 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* c0 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* d0 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* e0 */
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 /* f0 */
		};

		_bytes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		for (std::size_t i = 0; i < source.length(); ++i) {
			const signed char v(hex.at(static_cast<unsigned char>(source.at(i))));
			if (v < 0) {
				_bytes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				return;
			}
			_bytes[i / 2] |= static_cast<unsigned char>(v << (i % 2 ? 0 : 4));
		}
	}

	void oid::zero()
	{
		_bytes =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	}

	unsigned char oid::operator[](std::size_t index) const
	{
		return _bytes.at(index);
	}

	oid& oid::operator=(const oid& origin)
	{
		_bytes = origin._bytes;
		return *this;
	}

	oid& oid::operator=(oid&& origin)
	{
		_bytes = std::move(origin._bytes);
		return *this;
	}

	bool oid::operator<(const oid& origin) const
	{
		/// @xxx
		return (_bytes < origin._bytes);
	}

	bool oid::operator>(const oid& origin) const
	{
		/// @xxx
		return (_bytes > origin._bytes);
	}

	bool oid::operator==(const oid& rhs) const
	{
		return _bytes == rhs._bytes;
	}

	bool oid::operator!=(const oid& rhs) const
	{
		return !((*this) == rhs);
	}

	oid::operator const std::array<unsigned char, oid::size>&() const
	{
		return _bytes;
	}

	oid::operator std::array<unsigned char, oid::size>&()
	{
		return _bytes;
	}

	oid::operator std::string() const
	{
		return make_string();
	}

	std::string oid::make_string() const
	{
		std::string retval(40, '0'); char* r(retval.data());
		constexpr std::array<char, 16> hex = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		for (std::size_t i = 0; i < _bytes.size(); i++) {
			*r++ = hex[_bytes.at(i) >> 4];
			*r++ = hex[_bytes.at(i) & 0xf];
		}
		return retval;
	}

	unsigned int oid::murmur_hash() const
	{
		const unsigned int magic(0x5bd1e995);
		const unsigned int seed(0);
		const int r(24);

		unsigned int length(_bytes.size());
		unsigned int retval(seed ^ 16);

		const unsigned char* data(_bytes.data());
		unsigned int byte;

		while (length >= 4)
		{
			byte  = data[0];
			byte |= data[1] << 8;
			byte |= data[2] << 16;
			byte |= data[3] << 24;

			byte *= magic;
			byte ^= byte >> r;
			byte *= magic;

			retval *= magic;
			retval ^= byte;

			data += 4;
			length -= 4;
		}

		switch (length)
		{
		  case 3:
			retval ^= data[2] << 16;
		  case 2:
			retval ^= data[1] << 8;
		  case 1:
			retval ^= data[0];
			retval *= magic;
		};

		retval ^= retval >> 13;
		retval *= magic;
		retval ^= retval >> 15;

		return retval;
	}

	inline bool oid::is_zero() const
	{
		return (_bytes.at(0) == 0 && std::equal(_bytes.cbegin() + 1, _bytes.cend(), _bytes.cbegin()));
	}

	std::istream& operator>>(std::istream& stream, chaos::oid& oid)
	{
		oid.parse(stream);
		return stream;
	}
}

namespace std {
	uintptr_t hash<chaos::oid>::operator()(const chaos::oid& oid) const
	{
		return oid.is_zero() ? 0 : oid.murmur_hash();
	}
}

