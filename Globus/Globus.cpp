/**
 @file Globus.cpp
 @date 22.05.01
 @copyright Cadabra Systems
 @author daniil@megrabyan.pro
*/

#include "Globus.hpp"

namespace chaos { namespace globus {
	void ICUTextDeleter::operator()(UText* pointer)
	{
		utext_close(pointer);
	}

	void ICUBreakIteratorDeleter::operator()(UBreakIterator* pointer)
	{
		ubrk_close(pointer);
	}
} }
