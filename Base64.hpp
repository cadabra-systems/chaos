/**
 @file Base64.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Chaos_Base64_hpp
#define Chaos_Base64_hpp

#include <string>

namespace chaos {
	/**
	 @brief Base64 де/кодер
	 */
	class base64
	{
	/** @name Constants */
	/** @{ */
	public:
		const static std::string chars;
	/** @} */
		
	/** @name Converters */
	/** @{ */
	public:
		static bool valid(unsigned char c);
		static std::string encode(const std::string& data);
		static std::string encode(unsigned char const* data, size_t lenth);
		static std::string decode(std::string const& data);
	/** @} */
	};
}

#endif
