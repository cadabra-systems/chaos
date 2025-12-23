/**
 @file String.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_String_hpp
#define Chaos_String_hpp

#include <algorithm>
#include <string>
#include <bitset>
#include <vector>
#include <list>
#include <iterator>
#include <functional>
#include <regex>
#include <sstream>
#include <iomanip>
#include <set>

namespace chaos {
	class string {
	/** @name Classes */
	/** @{ */
	public:
		enum class trim_side
		{
			both = 0,
			left = 1,
			right = 2
		};

		class constant
		{
		public:
			template <std::size_t N>
			constexpr constant(const char(&a)[N])
			:
				_pointer(a),
				_size(N - 1)
			{
			}

		private:
			const char* const _pointer;
			const std::size_t _size;

		public:
			constexpr char operator[](std::size_t index) const
			{
				return index < _size ? _pointer[index] : throw std::out_of_range("");
			}

			constexpr std::size_t size() const
			{
				return _size;
			}

			constexpr const char* get() const
			{
				return _pointer;
			}
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		/**
		 * @brief empty
		 */
		static const std::string empty;

		template<typename S>
		static std::string convert(const S& value)
		{
			if constexpr (std::is_same_v<S, std::string>) {
				return value;
			} else if constexpr (std::is_same_v<S, char>) {
				return std::string(1, value);
			}
			return std::to_string(value);
		}

		/**
		 * @brief std::list<string> v = explode<std::list, std::string>("Hello, there; Ace", ";,");
		 */
		template<template<class, class> class C, class T>
		static C<T, std::allocator<T>> explode(const T& input, const T& delimiter)
		{
			C<T, std::allocator<T>> retval;

			typename T::size_type start = 0;
			auto pos = input.find_first_of(delimiter, start);
			while (pos != T::npos) {
				if(pos != start) {
					retval.emplace_back(input, start, pos - start);
				}

				start = pos + 1;
				pos = input.find_first_of(delimiter, start);
			}

			if (start < input.length()) {
				retval.emplace_back(input, start, input.length() - start);
			}

			return std::move(retval);
		}

		template<template<class, class, class> class C, class T>
		static C<T, std::less<T>, std::allocator<T>> explode(const T& input, const T& delimiter)
		{
			std::set<T, std::less<T>, std::allocator<T>> retval;

			typename T::size_type start = 0;
			auto pos = input.find_first_of(delimiter, start);
			while (pos != T::npos) {
				if(pos != start) {
					retval.emplace(input, start, pos - start);
				}

				start = pos + 1;
				pos = input.find_first_of(delimiter, start);
			}


			if (start < input.length()) {
				retval.emplace(input, start, input.length() - start);
			}

			return std::move(retval);
		}

		template <template<class, class> class C, class T>
		static inline T join(const typename C<T, std::allocator<T>>::const_iterator& begin, const typename C<T, std::allocator<T>>::const_iterator& end, const T& delimeter)
		{
			T retval;
			for (typename C<T, std::allocator<T>>::const_iterator t=begin; t != end; ++t) {
				if (!retval.empty()) {
					retval.append(delimeter);
				}
				retval.append(*t);
			}

			return retval;
		}

		static inline std::pair<std::string, std::string> split(const std::string& input, const std::string& delimiter)
		{
			const std::string::size_type d(input.find_first_of(delimiter));
			return (std::string::npos == d) ? std::make_pair(input, std::string{}) : std::make_pair(std::string{input, 0, d}, std::string{input, d + 1});
		}

		/**
		 * @brief Chops off the delimiter and everything that follows (destructively)
		 * @param subject
		 * @param offset
		 * @param length
		 * @param keep_delimeter
		 * @return everything after the delimiter
		 */
		static inline std::string slice_tail_string(std::string& subject, std::string::size_type offset, std::string::size_type length, bool keep_delimiter = false)
		{
			std::string output;
			if (offset < std::string::npos) {
				const std::string::size_type start(keep_delimiter ? offset : offset + length);
				const std::string::size_type end(subject.length() - start);
				output = subject.substr(start, end);
				subject = subject.substr(0, offset);
			}
			return output;
		}

		/**
		 * @brief Chops off the delimiter and everything that follows (destructively)
		 * @param subject
		 * @param delimiter
		 * @param keep_delimeter
		 * @return everything after the delimiter
		 */
		static inline std::string slice_tail_string(std::string& subject, const std::string& delimiter, bool keep_delimiter = false)
		{
			return slice_tail_string(subject, subject.find(delimiter), delimiter.length(), keep_delimiter);
		}

		/**
		 * @brief Chops off the delimiter and everything that precedes (destructively)
		 * @param subject
		 * @param offset
		 * @param length
		 * @return everthing before the delimeter
		 */
		static inline std::string slice_head_string(std::string& subject, std::string::size_type offset, std::string::size_type length)
		{
			std::string output;
			if (offset < std::string::npos) {
				output = subject.substr(0, offset);
				subject = subject.substr(offset + length, subject.length() - (offset + length));
			}
			return output;
		}

		/**
		 * @brief Chops off the delimiter and everything that precedes (destructively)
		 * @param subject
		 * @param delimiter
		 * @return everthing before the delimeter
		 */
		static inline std::string slice_head_string(std::string& subject, const std::string& delimiter)
		{
			return slice_head_string(subject, subject.find(delimiter), delimiter.length());
		}

		static inline std::string& trim_string(std::string& s, trim_side side = trim_side::both)
		{
			if (trim_side::right == side || trim_side::both == side) {
				s.erase(
						std::find_if(
									 s.rbegin(),
									 s.rend(),
									 [](int c)
									 {
										 return !std::isspace(c);
									 }
						).base(),
						s.end()
				);
			}
			if (trim_side::left == side || trim_side::both == side) {
				s.erase(
						s.begin(),
						std::find_if(
									 s.begin(),
									 s.end(),
									 [](int c)
									 {
										 return !std::isspace(c);
									 }
						)
				);
			}
			return s;
		}

		static inline std::string trimmed_string(const std::string& s, trim_side side = trim_side::both)
		{
			std::string retval(s);
			return trim_string(retval, side);
		}

		static inline std::string lowercased_string(std::string subject)
		{
			std::transform(subject.begin(), subject.end(), subject.begin(), ::tolower);

			return subject;
		}

		static inline std::string uppercased_string(std::string subject)
		{
			std::transform(subject.begin(), subject.end(), subject.begin(), ::toupper);

			return subject;
		}

		/**
		 * @brief formatted_string
		 * @param format
		 * @param args
		 * @example %d -> for int
		 * @example %u -> for unsigned int
		 * @example %ld-> for long int or long
		 * @example %lu-> for unsigned long int or long unsigned int or unsigned long
		 * @example %lld--> for long long int or long long
		 * @example %llu-> for unsigned long long int or unsigned long long
		 * @return formatted string
		 */
		template<typename... Args>
		static std::string formatted_string(const std::string& format, Args ... args)
		{
			const size_t size(snprintf(nullptr, 0, format.c_str(), args ...) + 1); // Extra space for '\0'
			const std::unique_ptr<char[]> buf(new char[size]);
			snprintf(buf.get(), size, format.c_str(), args ...);

			return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
		}

		template<typename T>
		static std::string number_to_bin_string(const T& i, bool prefix = true)
		{
			return (prefix ? "0b" : "") + std::bitset<sizeof(i)>(i).to_string();
		}

		template<typename T>
		static std::string number_to_hex_string(const T& i, bool prefix = true)
		{
			std::stringstream stream;
			stream << (prefix ? "0x" : "") << std::setfill ('0') << std::setw(sizeof(T)*2) << std::hex << i;
			return stream.str();
		}

		static inline std::string escaped_string(const std::string& source, const std::string& needle, const std::string& escape = "\\")
		{
			if (escape.length() == 0 || needle.length() == 0) {
				return source;
			}
			const std::string replacement(escape + needle);
			std::string retval(source);
			std::size_t offset(0);
			while ((offset = retval.find(needle, offset)) != std::string::npos) {
				retval.replace(offset, needle.length(), replacement);
				offset += replacement.length();
			}
			return retval;
		}

		static inline bool replace(std::string& source, const std::string& needle, const std::string& value)
		{
			const size_t offset(source.find(needle));
			if (std::string::npos == offset) {
				return false;
			}
			source.replace(offset, needle.length(), value);
			return true;
		}

		static inline bool replace_all(std::string& source, const std::string& needle, const std::string& value)
		{
			size_t offset(source.find(needle));
			if (std::string::npos == offset) {
				return false;
			}
			do {
				source.replace(offset, needle.length(), value);
				offset += value.length();
			} while ((offset = source.find(needle, offset)) != std::string::npos);
			return true;
		}

		static inline std::string replaced(const std::string& source, const std::string& needle, const std::string& value)
		{
			std::string retval(source);
			replace(retval, needle, value);
			return retval;
		}

		static inline std::string replaced_all(const std::string& source, const std::string& needle, const std::string& value)
		{
			std::string retval(source);
			replace_all(retval, needle, value);
			return retval;
		}

		static inline bool is_integer(const std::string& s)
		{
			if (s.empty()) {
				return false;
			} else if (s.front() == '-') {
				return (s.length() > 1 && std::find_if(std::next(s.cbegin()), s.cend(), [](char c) { return !std::isdigit(c); }) == s.end());
			} else {
				return std::find_if(s.cbegin(), s.cend(), [](char c) { return !std::isdigit(c); }) == s.end();
			}
		}

		static inline bool is_positive_integer(const std::string& s)
		{
			return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
		}

		static inline bool is_negative_integer(const std::string& s)
		{
			return s.length() > 1 && s.front() == '-' && std::find_if(std::next(s.begin()), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
		}

		static inline bool is_double(const std::string& s)
		{
			const std::string::const_iterator d(std::find(s.cbegin(), s.cend(), '.'));
			if (s.cend() == d) {
				return false;
			} else if (s.front() == '-') {
				return (
						s.length() > 2
						&&
						std::find_if(std::next(s.cbegin()), d, [](char c) { return !std::isdigit(c); }) == s.end()
						&&
						std::find_if(s.cbegin(), d, [](char c) { return !std::isdigit(c); }) == s.end()
				);
			} else {
				return (
						std::find_if(s.cbegin(), d, [](char c) { return !std::isdigit(c); }) == s.end()
						&&
						std::find_if(d, s.cend(), [](char c) { return !std::isdigit(c); }) == s.end()
				);
			}
		}

		static inline bool one_of(const std::string& value, const std::list<std::string>& list)
		{
			return std::find(list.begin(), list.end(), value) != list.end();
		}

		/**
		 * @brief wild_match
		 * @param target
		 * @param pattern
		 * @author Jack Handy <jakkhandy@hotmail.com>
		 * @url https://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
		 * @return
		 */
		static bool wild_match(const char* target, const char* pattern);

		static inline bool wild_match(const std::string& target, const std::string& pattern)
		{
			return wild_match(target.data(), pattern.data());
		}

		/**
		 * @brief fastwild_match
		 * @param target
		 * @param pattern
		 * @author Kirk J. Krauss
		 * @url https://github.com/kirkjkrauss/MatchingWildcards/blob/master/Listing2.cpp
		 * @return bool
		 */
		static bool fastwild_match(const char* target, const char* pattern);

		static inline bool fastwild_match(const std::string& target, const std::string& pattern)
		{
			return fastwild_match(target.data(), pattern.data());
		}
	/** @} */
	};
}
#endif
