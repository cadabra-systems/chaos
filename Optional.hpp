#ifndef Cadabra_Chaos_Optional_hpp
#define Cadabra_Chaos_Optional_hpp

#ifdef __has_include /// < Check if __has_include is present
#if __has_include(<optional>) /// < Check for a standard library
#include<optional>
#elif __has_include(<experimental/optional>) /// < Check for an experimental version
#include <experimental/optional>
namespace std {
	using optional = std::experimental::optional;
	using nullopt = std::experimental::nullopt;
}
#elif __has_include(<boost/optional.hpp>) /// < Try with an external library
#include <boost/optional.hpp>
#else /// < Not found at all
#error "Missing <optional>"
#endif
#endif

#endif
