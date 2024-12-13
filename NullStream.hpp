/**
 @file NullStream.hpp
 @date 20.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Chaos_NullStream_hpp
#define Chaos_NullStream_hpp

#include <iostream>

namespace chaos {
	template<typename Ch, typename Traits = std::char_traits<Ch> >
	struct basic_nullbuf : std::basic_streambuf<Ch, Traits>
	{
		using base_type = std::basic_streambuf<Ch, Traits>;
		using int_type = typename base_type::int_type;
		using traits_type = typename base_type::traits_type;

		virtual int_type overflow(int_type c) override
		{
			return traits_type::not_eof(c);
		}

		virtual std::streamsize xsputn(const typename std::basic_streambuf<Ch, Traits>::char_type*, std::streamsize count) override
		{
			return count;
		}
	};

	using nullbuf = basic_nullbuf<char>;
	using wnullbuf = basic_nullbuf<wchar_t>;

	inline static thread_local nullbuf null_obj;
	inline static thread_local wnullbuf wnull_obj;

	inline static thread_local std::ostream cnull(&null_obj);
	inline static thread_local std::wostream wcnull(&wnull_obj);
}

#endif
