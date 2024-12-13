/**
 @file String.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "String.hpp"

#include "G11N.hpp"

#include <cstdint>
#include <limits>
#include <memory>

#include <unicode/utf8.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/utext.h>
#include <unicode/ubrk.h>

namespace chaos { namespace g11n {
	std::size_t string::count(const std::string& data, text_item counter_type)
	{
		return string::count(data.data(), data.length(), counter_type);
	}

	std::size_t string::count(const char* data, std::size_t size, text_item counter_type)
	{
		std::size_t count(0);
		UErrorCode status(U_ZERO_ERROR);
		ICUText text(utext_openUTF8(nullptr, data, size, &status));
		if (!U_SUCCESS(status)) {
			return 0;
		} else if (text_item::codepoint == counter_type) {
			while (utext_next32(text.get()) != U_SENTINEL) {
				++count;
			}
			return count;
		} else if (text_item::white_space == counter_type) {
			while (utext_next32(text.get()) != U_SENTINEL) {
				if (u_isUWhiteSpace(utext_current32(text.get()))) {
					++count;
				}
			}
			return count;
		}
		ICUBreakIterator i(ubrk_open(UBRK_WORD, "en_us", nullptr, 0, &status));
		ubrk_setUText(i.get(), text.get(), &status);
		if (!U_SUCCESS(status)) {
			return 0;
		} else if (text_item::word == counter_type) {
			while (ubrk_next(i.get()) != UBRK_DONE) {
				if (ubrk_getRuleStatus(i.get()) != UBRK_WORD_NONE) {
					/// @note Count only words and numbers, not spaces or punctuation
					++count;
				}
			}
		} else if (text_item::grapheme == counter_type) {
			/// @bug
			while (ubrk_next(i.get()) != UBRK_DONE) {
				++count;
			}
		}
		/// @note Reset the index so we can use the structure again
//			UTEXT_SETNATIVEINDEX(text.get(), 0);
		return U_SUCCESS(status) ? count : 0;
	}

	string::string()
	:
		_data(nullptr),
		_capacity(0)
	{

	}

	string::string(const std::string& input)
	:
		string(input.data(), input.length())
	{

	}

	string::string(const char* input, std::size_t size)
	:
		_data(nullptr),
		_capacity(0)
	{
		/**
		 * @brief
		 * If *target is NULL determine required destination buffer size (pre-flighting).
		 * Otherwise, attempt to convert source string; if *target buffer is not large enough
		 * it will be resized appropriately.
		 */
		UErrorCode status(U_ZERO_ERROR);
		if (size > std::numeric_limits<std::int32_t>::max()) {
			/// @note we cannot fit this string
			status = U_BUFFER_OVERFLOW_ERROR;
			return;
		}

		int32_t destination_size(0);
		u_strFromUTF8(_data, _capacity, &destination_size, input, static_cast<int32_t>(size), &status);
		if (U_ZERO_ERROR == status) { /// < String is converted successfully
			_data[destination_size] = 0;
			_capacity = destination_size;
			return;
		}

		/**
		 * @brief
		 * Bail out if an unexpected error occurred:
		 * U_BUFFER_OVERFLOW_ERROR means that _data buffer is not large enough
		 * U_STRING_NOT_TERMINATED_WARNING usually means that the input string is empty
		 */
		if (
			U_BUFFER_OVERFLOW_ERROR != status
			&&
			U_STRING_NOT_TERMINATED_WARNING != status
		) {
			return;
		}

		/// @note Convert source string from UTF-8 to UTF-16
		status = U_ZERO_ERROR;

		/// @note Allocate memory for the destination buffer (it will be zero-terminated)
		UChar* destination_buffer(new UChar[destination_size + 1]);
		std::fill(destination_buffer, destination_buffer + destination_size, '\0');
		u_strFromUTF8(destination_buffer, destination_size + 1, nullptr, input, static_cast<int32_t>(size), &status);
		if (U_FAILURE(status)) {
			delete[] destination_buffer;
			return;
		} else if (*_data) {
			delete[] _data;
		}
		destination_buffer[destination_size] = 0;
		_data = destination_buffer;
		_capacity = destination_size;
	}

	string::~string()
	{
		if (*_data) {
			delete[] _data;
		}
	}

	void string::zero()
	{

	}

	string::operator std::string() const
	{
		return "";
	}

	std::size_t string::size() const
	{
		return 0;
	}

	std::size_t string::length() const
	{
		return 0;
	}

	bool string::whitespace(std::int32_t offset) const
	{
		return false;
	}
} }
