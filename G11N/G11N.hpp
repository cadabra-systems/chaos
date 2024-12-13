/**
 @file G11N.hpp
 @date 2022-05-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_G11N_G11N_hpp
#define Chaos_G11N_G11N_hpp

#include <memory>

#include <unicode/utext.h>
#include <unicode/ubrk.h>

namespace chaos { namespace g11n {
	struct ICUTextDeleter
	{
		void operator()(UText* pointer);
	};

	struct ICUBreakIteratorDeleter
	{
		void operator()(UBreakIterator* pointer);
	};

	using ICUText = std::unique_ptr<UText, ICUTextDeleter>;
	using ICUBreakIterator = std::unique_ptr<UBreakIterator, ICUBreakIteratorDeleter>;
} }

#endif
