/**
 @file StringList.cpp
 @date 26.03.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#include "StringList.hpp"

namespace chaos {
	string_list& string_list::operator<<(const std::list<std::string>& list)
	{
		std::stringstream& stream(_stream);
		std::for_each(list.cbegin(), list.cend(), [&stream](const std::string& s){ stream << s; });

		return *this;
	}

	string_list& string_list::operator<<(const string_list& list)
	{
		_stream << list._stream.rdbuf();
		
		return *this;
	}

	string_list& string_list::operator<<(std::ostream& (*f)(std::ostream&))
	{
		_stream << f;
		
		return *this;
	}
	
	const std::stringstream& string_list::stream() const
	{
		return _stream;
	}
}
