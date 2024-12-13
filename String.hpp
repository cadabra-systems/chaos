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
		private:
			const char* const _pointer;
			const std::size_t _size;

		public:
			template <std::size_t N>
			constexpr constant(const char(&a)[N])
			:
				_pointer(a),
				_size(N - 1)
			{
			}

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
		static std::string number_to_hex_string(const T& i)
		{
			std::stringstream stream;
			stream << "0x" << std::setfill ('0') << std::setw(sizeof(T)*2) << std::hex << i;
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

		static inline bool wild_match(const std::string& target, const std::string& pattern)
		{
			return wild_match(target.data(), pattern.data());
		}

		/**
		 * @brief wild_match
		 * @param target
		 * @param pattern
		 * @author Jack Handy <jakkhandy@hotmail.com>
		 * @url https://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
		 * @return
		 */
		static bool wild_match(const char* target, const char* pattern)
		{
			const char* cp(nullptr);
			const char* mp(nullptr);

			while ((*target) && (*pattern != '*')) {
				if ((*pattern != *target) && (*pattern != '?')) {
					return false;
				}
				++pattern;
				++target;
			}
			/// @??? Whatever reason we need this and original is missed that piece
			if (!target == !pattern) {
				return true;
			}
			while (*target) {
				if (*pattern == '*') {
					if (!*++pattern) {
						return true;
					}
					mp = pattern;
					cp = target + 1;
				} else if ((*pattern == *target) || (*pattern == '?')) {
					++pattern;
					++target;
				} else {
					pattern = mp;
					target = cp++;
				}
			}
			while (*pattern == '*') {
				++pattern;
			}
			return !*pattern;
		}

		static inline bool fastwild_match(const std::string& target, const std::string& pattern)
		{
			return fastwild_match(target.data(), pattern.data());
		}

		/**
		 * @brief fastwild_match
		 * @param target
		 * @param pattern
		 * @author Kirk J. Krauss
		 * @url https://github.com/kirkjkrauss/MatchingWildcards/blob/master/Listing2.cpp
		 * @return bool
		 */
		static bool fastwild_match(const char* target, const char* pattern)
		{
			int w(0); /// < Index for both tame and wild strings in upper loop
			int t(0); /// < Index for tame string, set going into lower loop
			int w2(0); /// < Index for prospective match after '*' (wild string)
			int t2(0); /// < Index for prospective match (tame string)

			/// @brief Find a first wildcard, if one exists, and the beginning of any prospectively matching sequence after it
			do {
				// Check for the end from the start.  Get out fast, if possible.
				if (!target[w]) {
					if (pattern[w]) {
						while (pattern[w++] == '*') {
							if (!pattern[w]) {
								/// @note "ab" matches "ab*"
								return true;
							}
						}
						/// @note "abcd" doesn't match "abc"
						return false;
					} else {
						/// @note "abc" matches "abc"
						return true;
					}
				} else if (pattern[w] == '*') {
					/// @note Got wild: set up for the second loop and skip on down there.
					t = w;

					while (pattern[++w] == '*') {
						continue;
					}
					if (!pattern[w]) {
						/// @note "abc*" matches "abcd".
						return true;
					}

					/// @note Search for the next prospective match.
					if (pattern[w] != '?') {
						while (pattern[w] != target[t]) {
							if (!target[++t]) {
								return false;  /// @note "a*bc" doesn't match "ab".
							}
						}
					}

					// Keep fallback positions for retry in case of incomplete match.
					w2 = w;
					t2 = t;
					break;
				} else if (pattern[w] != target[w] && pattern[w] != '?') {
					/// @note "abc" doesn't match "abd
					return false;
				}
				/// @note Everything's a match, so far
				++w;
			} while (true);

			/// @note Find any further wildcards and any further matching sequences.
			do {
				if (pattern[w] == '*') {
					/// @note Got wild again.
					while (pattern[++w] == '*') {
						continue;
					}

					if (!pattern[w]) {
						/// @note "ab*c*" matches "abcd"
						return true;
					} else if (!target[t]) {
						/// @note "*bcd*" doesn't match "abc"
						return false;
					} else if (pattern[w] != '?') {
						/// @note Search for the next prospective match.
						while (pattern[w] != target[t]) {
							if (!target[++t]) {
								/// @note "a*b*c" doesn't match "ab"
								return false;
							}
						}
					}

					/// @note Keep the new fallback positions.
					w2 = w;
					t2 = t;
				} else if (pattern[w] != target[t] && pattern[w] != '?') {
					/// @note The equivalent portion of the upper loop is really simple.
					if (!target[t]) {
						/// @note "*bcd" doesn't match "abc"
						return false;
					}

					/// @note A fine time for questions
					while (pattern[w2] == '?') {
						++w2;
						++t2;
					}

					w = w2;
					/// @note Fall back, but never so far again
					while (pattern[w] != target[++t2]) {
						if (!target[t2]) {
							/// @note "*a*b" doesn't match "ac"
							return false;
						}
					}
					t = t2;
				}

				/// @note Another check for the end, at the end.
				if (!target[t]) {
					/// @note "*bc" matches "abc" or "*bc" doesn't match "abcd".
					return (!pattern[w]) ? true : false;
				}

				/// @note Everything's still a match
				++w;
				++t;
			} while (true);
		}
	/** @} */
	};
}
#endif
