/**
 @file Color.hpp
 @date 09.03.17
 @copyright Cadabra / Ace
 @author Daniil A Megrabyan
*/

#ifndef Chaos_Color_hpp
#define Chaos_Color_hpp

#include <map>
#include <string>

namespace chaos {
	class color
	{
	/** @name Static */
	/** @{ */
	public:
		static const std::map<std::string, std::int32_t> map;

		static const std::int32_t clear = 0x1000000;
		static const std::int32_t black = 0x000000;
		static const std::int32_t white = 0xFFFFFF;
		static const std::int32_t blue = 0x0000FF;
		static const std::int32_t red = 0xFF0000;
		static const std::int32_t orange = 0xFFA500;
		static const std::int32_t lime = 0x00FF00;
		static const std::int32_t yellow = 0xFFFF00;
		static const std::int32_t cyan = 0x00FFFF;
		static const std::int32_t magenta = 0xFF00FF;
		static const std::int32_t silver = 0xC0C0C0;
		static const std::int32_t gray = 0x808080;
		static const std::int32_t maroon = 0x800000;
		static const std::int32_t olive = 0x808000;
		static const std::int32_t green = 0x008000;
		static const std::int32_t purple = 0x800080;
		static const std::int32_t teal = 0x008080;
		static const std::int32_t navy = 0x000080;
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		color();
		virtual ~color() = default;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
	/** @} */

	/** @name Properties */
	/** @{ */
	private:

	protected:
	/** @} */

	/** @name Procedures  */
	/** @{ */
	public:
	/** @} */

	/** @name Converters */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		static std::int32_t get(std::string name);
	/** @} */

	/** @name Formatters */
	/** @{ */
	public:
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
	/** @} */

	/** @name Templates */
	/** @{ */
	public:
	/** @} */
	};
}

#endif
