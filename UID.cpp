//
//  UID.cpp
//
//  Created by Daniil A Megrabyan on 26.08.16.
//  Copyright © 2016 Cadabra Systems. All rights reserved.
//

#include "UID.hpp"

namespace chaos {
	uid::uid(const std::string& source)
	{
		parse(source);
	}
	
	uid::uid(std::array<unsigned char, 16> source)
	:
		_bytes(source)
	{

	}

	uid::uid(uid&& origin)
	:
		_bytes(std::move(origin._bytes))
	{

	}
	
	uid::uid(const uid& origin)
	:
		_bytes(origin._bytes)
	{

	}
	
	uid::uid(bool random)
	{
		if (random) {
			generate();
		} else {
			zero();
		}
	}

	uid::uid()
	:
		_bytes({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
	{

	}

	void uid::generate()
	{
#ifdef CHAOS_UUID
		uuid_t id;
		uuid_generate(id);
		std::memcpy(_bytes.data(), id, _bytes.size());
		
#elif CHAOS_GUID
		GUID id;
		CoCreateGuid(&id);
		
		_bytes =
		{
			(id.Data1 >> 24) & 0xFF,
			(id.Data1 >> 16) & 0xFF,
			(id.Data1 >> 8) & 0xFF,
			(id.Data1) & 0xff,
			
			(id.Data2 >> 8) & 0xFF,
			(id.Data2) & 0xff,
			
			(id.Data3 >> 8) & 0xFF,
			(id.Data3) & 0xFF,
			
			id.Data4[0],
			id.Data4[1],
			id.Data4[2],
			id.Data4[3],
			id.Data4[4],
			id.Data4[5],
			id.Data4[6],
			id.Data4[7]
		};
#endif
	}

	void uid::parse(std::istream& stream)
	{
		std::string part;
		stream >> part;
		parse(part);
	}

	void uid::parse(const std::string& source)
	{
		_bytes = std::array<unsigned char, 16>({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});

		std::string::const_iterator i(source.cbegin());
		if (source.length() == 38 && *(source.cbegin()) == '{' && *(source.rbegin()) == '}') {
			++i;
		} else if (source.length() != 36) {
			return ;
		}

		for (unsigned char& b : _bytes) {
			if ((*i) == '-') {
				++i;
			}

			b = convert(*i) * 16 + convert(*(i+1));
			i += 2 ;
		}
	}
	
	void uid::zero()
	{
		_bytes = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
	}

	unsigned char uid::operator[](std::size_t index) const
	{
		return _bytes.at(index);
	}

	uid& uid::operator=(const uid& origin)
	{
		_bytes = origin._bytes;
		return *this;
	}

	uid& uid::operator=(uid&& origin)
	{
		_bytes = std::move(origin._bytes);
		return *this;
	}
	
	bool uid::operator<(const uid& origin) const
	{
		/// @xxx
		return (_bytes < origin._bytes);
	}

	bool uid::operator<=(const uid& origin) const
	{
		/// @xxx
		return (_bytes <= origin._bytes);
	}
	
	bool uid::operator>(const uid& origin) const
	{
		/// @xxx
		return (_bytes > origin._bytes);
	}

	bool uid::operator>=(const uid& origin) const
	{
		/// @xxx
		return (_bytes >= origin._bytes);
	}
	
	bool uid::operator==(const uid& rhs) const
	{
		return _bytes == rhs._bytes;
	}
	
	bool uid::operator!=(const uid& rhs) const
	{
		return !((*this) == rhs);
	}

	uid::operator const std::array<unsigned char, uid::size>&() const
	{
		return _bytes;
	}

	uid::operator std::array<unsigned char, uid::size>&()
	{
		return _bytes;
	}

	uid::operator std::string() const
	{
		return make_string();
	}

	unsigned char uid::convert(char c) const
	{
		if (c > 47 && c < 58) {
			return c - 48;
		}

		if (c > 96 && c < 103) {
			return c - 87;
		}

		if (c > 64 && c < 71) {
			return c - 55;
		};

		return 0;
	}

	std::string uid::make_string() const
	{
		std::stringstream stream;
		
		stream << std::hex << std::setfill('0')
		<< std::setw(2) << static_cast<int>(_bytes[0])
		<< std::setw(2) << static_cast<int>(_bytes[1])
		<< std::setw(2) << static_cast<int>(_bytes[2])
		<< std::setw(2) << static_cast<int>(_bytes[3])
		<< "-"
		<< std::setw(2) << static_cast<int>(_bytes[4])
		<< std::setw(2) << static_cast<int>(_bytes[5])
		<< "-"
		<< std::setw(2) << static_cast<int>(_bytes[6])
		<< std::setw(2) << static_cast<int>(_bytes[7])
		<< "-"
		<< std::setw(2) << static_cast<int>(_bytes[8])
		<< std::setw(2) << static_cast<int>(_bytes[9])
		<< "-"
		<< std::setw(2) << static_cast<int>(_bytes[10])
		<< std::setw(2) << static_cast<int>(_bytes[11])
		<< std::setw(2) << static_cast<int>(_bytes[12])
		<< std::setw(2) << static_cast<int>(_bytes[13])
		<< std::setw(2) << static_cast<int>(_bytes[14])
		<< std::setw(2) << static_cast<int>(_bytes[15]);
		
		return stream.str();
	}

	unsigned int uid::murmur_hash() const
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

	bool uid::is_zero() const
	{
		constexpr std::array<unsigned char, 16> zero_array({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
		return _bytes == zero_array;
	}

	std::istream& operator>>(std::istream& stream, chaos::uid& uid)
	{		
		uid.parse(stream);
		return stream;
	}
}

namespace std {
	uintptr_t hash<chaos::uid>::operator()(const chaos::uid& uid) const
	{
		return uid.is_zero() ? 0 : uid.murmur_hash();
	}
}
