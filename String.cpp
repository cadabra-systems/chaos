/**
 @file String.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "String.hpp"

namespace chaos {
	const std::string string::empty = "";

	bool string::wild_match(const char* target, const char* pattern)
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

	bool string::fastwild_match(const char* target, const char* pattern)
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
}
