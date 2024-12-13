/**
 @file AbstractDeserializer.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_AbstractDeserializer_hpp
#define Chaos_AbstractDeserializer_hpp

#include "SerializerException.hpp"
#include "Type.hpp"
#include "Hash.hpp"

#include <istream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace chaos {
	class abstract_deserializer
	{
	/** @name Constructors */
	/** @{ */
	public:
		abstract_deserializer(std::istream& stream);
		virtual ~abstract_deserializer() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
		std::istream& _stream;
		std::size_t _length;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		std::istream& get_stream();
		std::size_t get_position() const;
	/** @} */
		
	/** @name Procedures */
	/** @{ */
	protected:
		virtual unsigned char read_byte() final;
		virtual std::string read_bytes(long length) final;

	public:
		virtual std::uint8_t read_marker() final;
		virtual std::int64_t read_id() = 0;
		virtual std::uint32_t read_size() = 0;
		virtual std::uint64_t read_index() = 0;
		virtual std::string read_blob() = 0;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		virtual void operator>>(std::uint8_t& value) final;

		virtual void operator>>(std::string& value) = 0;
		virtual void operator>>(std::uint16_t& value) = 0;
		virtual void operator>>(std::int16_t& value) = 0;
		virtual void operator>>(std::uint32_t& value) = 0;
		virtual void operator>>(std::int32_t& value) = 0;
		virtual void operator>>(std::uint64_t& value) = 0;
		virtual void operator>>(std::int64_t& value) = 0;
		virtual void operator>>(double& value) = 0;
		virtual void operator>>(bool& value) = 0;
		virtual void operator>>(std::chrono::system_clock::time_point& value) = 0;
	/** @} */
	};
}

#endif
