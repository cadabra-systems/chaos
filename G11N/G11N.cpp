/**
 @file G11N.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "G11N.hpp"

namespace chaos { namespace g11n {
	void ICUTextDeleter::operator()(UText* pointer)
	{
		utext_close(pointer);
	}

	void ICUBreakIteratorDeleter::operator()(UBreakIterator* pointer)
	{
		ubrk_close(pointer);
	}
} }
