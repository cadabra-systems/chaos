/**
 @file AbstractDeserializer.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "AbstractDeserializer.hpp"

namespace chaos {
	abstract_deserializer::abstract_deserializer(std::istream& stream)
	:
		_stream(stream)
	{
		std::streampos restore = _stream.tellg();
		_stream.seekg(0, std::ios::end);
		_length = _stream.tellg();
		_stream.seekg(restore);
	}
	
	unsigned char abstract_deserializer::read_byte()
	{
		if (!_stream.good()) {
			throw exception_provoker<serializer_exception>(serializer_exception::code::stream_reading) << "Buffer underrun at needle position: " << _stream.tellg() << " while requesting length: 1" << std::endl;
		}
		
		return _stream.get();
	}
	
	std::string abstract_deserializer::read_bytes(const long length)
	{
		if (!_stream.good()) {
			throw exception_provoker<serializer_exception>(serializer_exception::code::stream_reading) << "Buffer underrun at needle position: " << _stream.tellg() << " while requesting length: " << length << std::endl;
		}
		
		char* buffer = new char[length];
		/// @bug Возможная утечка памяти если во время read() произошел Exception
		_stream.read(buffer, length);
		std::string retval(buffer, length);
		delete[] buffer;
		
		return retval;
	}
	
	std::uint8_t abstract_deserializer::read_marker()
	{
		return read_byte();
	}
	
	std::istream& abstract_deserializer::get_stream()
	{
		return _stream;
	}
	
	std::size_t abstract_deserializer::get_position() const
	{
		return _stream.tellg();
	}
	
	void abstract_deserializer::operator>>(std::uint8_t& value)
	{
		value =	read_byte();
	}
}
