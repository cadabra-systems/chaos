/**
 @file Exception.hpp
 @date 02.04.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_CMF_Exception_hpp
#define Chaos_CMF_Exception_hpp

#include "../Exception.hpp"

#include <cstdint>

namespace chaos { namespace cmf {
	class exception : public chaos::exception
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class code : std::uint8_t
		{
			unknown = 0x00,
			unknown_version = 0x01,
			proto_mismatch = 0x02,
			ambiguous_data = 0x03,
			bad_reference = 0x04,
			stream_writing = 0x10,
			stream_reading = 0x11,
			common_serializing = 0x20,
			common_deserializing = 0x21
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public :
		exception(exception::code code);
		virtual ~exception() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const exception::code _code;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const exception::code& reason() const;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
