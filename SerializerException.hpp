/**
 @file SerializerException.hpp
 @date 02.04.16
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_SerializerException_hpp
#define Chaos_SerializerException_hpp

#include "Exception.hpp"

namespace chaos {
	class serializer_exception : public exception
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class code : std::uint8_t
		{
			unknown = 0x00,
			stream_writing = 0x01,
			stream_reading = 0x02
		};
	/** @} */
		
	/** @name Constructors */
	/** @{ */
	public:
		serializer_exception(const serializer_exception::code code);
		virtual ~serializer_exception() = default;
	/** @} */
		
	/** @name Properties */
	/** @{ */
	private:
		const serializer_exception::code _code;
	/** @} */
		
	/** @name Getters */
	/** @{ */
	public:
		const serializer_exception::code& reason() const;
	/** @} */
		
	/** @name Operators */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
