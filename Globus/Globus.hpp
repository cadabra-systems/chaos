/**
 @file Globus.hpp
 @date 2022-05-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
 */

#ifndef Chaos_Globus_Globus_hpp
#define Chaos_Globus_Globus_hpp

#include <memory>

#include <unicode/utext.h>
#include <unicode/ubrk.h>

namespace chaos { namespace globus {
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
namespace chaos {
	namespace g11n = globus;
}
#endif
