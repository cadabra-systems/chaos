/**
 @file AbstractSerializer.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_AbstractSerializer_hpp
#define Chaos_AbstractSerializer_hpp

#include "Hash.hpp"
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace chaos {
	class abstract_serializer
	{
	/** @name Constructors */
	/** @{ */
	public:
		abstract_serializer(std::ostream& stream);
		virtual ~abstract_serializer() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	protected:
		std::ostream& _stream;
		std::size_t _offset;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		std::ostream& get_stream();
		std::size_t get_length() const;
		std::size_t get_position() const;
	/** @} */
	
	/** @name Procedures */
	/** @{ */
	protected:
		virtual void write_byte(const unsigned char byte) final;
		virtual void write_bytes(const std::string& bytes) final;
		
	public:
		virtual void write_marker(const std::uint8_t& marker) final;
		virtual void write_id(const std::int64_t id) = 0;
		virtual void write_size(const std::uint32_t& size) = 0;
		virtual void write_index(const std::uint64_t& size) = 0;
		virtual void write_blob(const std::string& value) = 0;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
		virtual void operator<<(const std::string& value) = 0;
		virtual void operator<<(const std::uint8_t& value) = 0;
		virtual void operator<<(const std::uint16_t& value) = 0;
		virtual void operator<<(const std::int16_t& value) = 0;
		virtual void operator<<(const std::uint32_t& value) = 0;
		virtual void operator<<(const std::int32_t& value) = 0;
		virtual void operator<<(const std::uint64_t& value) = 0;
		virtual void operator<<(const std::int64_t& value) = 0;
		virtual void operator<<(const double& value) = 0;
		virtual void operator<<(const bool& value) = 0;
		virtual void operator<<(const std::chrono::system_clock::time_point& value) = 0;
	/** @} */
	};
}

#endif
