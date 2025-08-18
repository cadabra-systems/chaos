/**
 @file String.hpp
 @date 2022-05-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Globus_String_hpp
#define Chaos_Globus_String_hpp

#include "Globus.hpp"

#include <string>
#include <unicode/utypes.h>

namespace chaos { namespace globus {
	class string
	{
	/** @name Statics */
	/** @{ */
	public:
		enum class text_item
		{
			codepoint,/// < fi
			grapheme, /// < f_i
			word, /// < final
			white_space
			/// @todo
		};
	/** @} */
	/** @name Statics */
	/** @{ */
	public:
		static std::size_t count(const std::string& data, text_item counter_type = text_item::grapheme);
		static std::size_t count(const char* data, std::size_t size = -1, text_item counter_type = text_item::grapheme);
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		string();
		string(const std::string& input);
		string(const char* data, std::size_t length);
		~string();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		UChar* _data;
		std::int32_t _capacity;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void zero();
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	private:
		char16_t* buffer(std::int32_t min_capacity);
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		operator std::string() const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		std::size_t size() const;
		std::size_t length() const;

		bool whitespace(std::int32_t offset) const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */
	};
} }
#endif
