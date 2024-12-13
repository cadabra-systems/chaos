/**
 @file AbstractSerializer.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "AbstractSerializer.hpp"

namespace chaos {
	abstract_serializer::abstract_serializer(std::ostream& stream)
	:
		_stream(stream),
		_offset(stream.tellp())
	{

	}
	
	void abstract_serializer::write_byte(const unsigned char byte)
	{
		_stream.put(byte);
	}
	
	void abstract_serializer::write_bytes(const std::string& bytes)
	{
		_stream.write(bytes.c_str(), bytes.length());
	}
	
	void abstract_serializer::write_marker(const std::uint8_t& marker)
	{
		write_byte(marker);
	}
	
	std::ostream& abstract_serializer::get_stream()
	{
		return _stream;
	}
	
	std::size_t abstract_serializer::get_length() const
	{
		return get_position() - _offset;
	}

	std::size_t abstract_serializer::get_position() const
	{
		return _stream.tellp();
	}
}
